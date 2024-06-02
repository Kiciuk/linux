// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#include <drm/drm_fourcc.h>
#include <drm/drm_util.h>

#include "dpu1_kms.h"
#include "dpu1_smp.h"


struct dpu1_smp {
	struct drm_device *dev;

	uint8_t reserved[MAX_CLIENTS]; /* fixed MMBs allocation per client */

	int blk_cnt;
	int blk_size;

	/* register cache */
	u32 alloc_w[22];
	u32 alloc_r[22];
	u32 pipe_reqprio_fifo_wm0[SSPP_MAX];
	u32 pipe_reqprio_fifo_wm1[SSPP_MAX];
	u32 pipe_reqprio_fifo_wm2[SSPP_MAX];
};

static inline
struct dpu1_kms *get_kms(struct dpu1_smp *smp)
{
	struct msm_drm_private *priv = smp->dev->dev_private;

	return to_dpu1_kms(to_mdp_kms(priv->kms));
}

static inline u32 pipe2client(enum dpu1_pipe pipe, int plane)
{
#define CID_UNUSED	0

	if (WARN_ON(plane >= pipe2nclients(pipe)))
		return CID_UNUSED;

	/*
	 * Note on SMP clients:
	 * For ViG pipes, fetch Y/Cr/Cb-components clients are always
	 * consecutive, and in that order.
	 *
	 * e.g.:
	 * if dpu1_cfg->smp.clients[SSPP_VIG0] = N,
	 *	Y  plane's client ID is N
	 *	Cr plane's client ID is N + 1
	 *	Cb plane's client ID is N + 2
	 */

	return dpu1_cfg->smp.clients[pipe] + plane;
}

/* allocate blocks for the specified request: */
static int smp_request_block(struct dpu1_smp *smp,
		struct dpu1_smp_state *state,
		u32 cid, int nblks)
{
	void *cs = state->client_state[cid];
	int i, avail, cnt = smp->blk_cnt;
	uint8_t reserved;

	/* we shouldn't be requesting blocks for an in-use client: */
	WARN_ON(!bitmap_empty(cs, cnt));

	reserved = smp->reserved[cid];

	if (reserved) {
		nblks = max(0, nblks - reserved);
		DBG("%d MMBs allocated (%d reserved)", nblks, reserved);
	}

	avail = cnt - bitmap_weight(state->state, cnt);
	if (nblks > avail) {
		DRM_DEV_ERROR(smp->dev->dev, "out of blks (req=%d > avail=%d)\n",
				nblks, avail);
		return -ENOSPC;
	}

	for (i = 0; i < nblks; i++) {
		int blk = find_first_zero_bit(state->state, cnt);
		set_bit(blk, cs);
		set_bit(blk, state->state);
	}

	return 0;
}

static void set_fifo_thresholds(struct dpu1_smp *smp,
		enum dpu1_pipe pipe, int nblks)
{
	u32 smp_entries_per_blk = smp->blk_size / (128 / BITS_PER_BYTE);
	u32 val;

	/* 1/4 of SMP pool that is being fetched */
	val = (nblks * smp_entries_per_blk) / 4;

	smp->pipe_reqprio_fifo_wm0[pipe] = val * 1;
	smp->pipe_reqprio_fifo_wm1[pipe] = val * 2;
	smp->pipe_reqprio_fifo_wm2[pipe] = val * 3;
}

/*
 * NOTE: looks like if horizontal decimation is used (if we supported that)
 * then the width used to calculate SMP block requirements is the post-
 * decimated width.  Ie. SMP buffering sits downstream of decimation (which
 * presumably happens during the dma from scanout buffer).
 */
uint32_t dpu1_smp_calculate(struct dpu1_smp *smp,
		const struct mdp_format *format,
		u32 width, bool hdecim)
{
	const struct drm_format_info *info = drm_format_info(format->base.pixel_format);
	struct dpu1_kms *dpu1_kms = get_kms(smp);
	int rev = dpu1_cfg_get_hw_rev(dpu1_kms->cfg);
	int i, hsub, nplanes, nlines;
	uint32_t blkcfg = 0;

	nplanes = info->num_planes;
	hsub = info->hsub;

	/* different if BWC (compressed framebuffer?) enabled: */
	nlines = 2;

	/* Newer MDPs have split/packing logic, which fetches sub-sampled
	 * U and V components (splits them from Y if necessary) and packs
	 * them together, writes to SMP using a single client.
	 */
	if ((rev > 0) && (format->chroma_sample > CHROMA_FULL)) {
		nplanes = 2;

		/* if decimation is enabled, HW decimates less on the
		 * sub sampled chroma components
		 */
		if (hdecim && (hsub > 1))
			hsub = 1;
	}

	for (i = 0; i < nplanes; i++) {
		int n, fetch_stride, cpp;

		cpp = info->cpp[i];
		fetch_stride = width * cpp / (i ? hsub : 1);

		n = DIV_ROUND_UP(fetch_stride * nlines, smp->blk_size);

		/* for hw rev v1.00 */
		if (rev == 0)
			n = roundup_pow_of_two(n);

		blkcfg |= (n << (8 * i));
	}

	return blkcfg;
}

int dpu1_smp_assign(struct dpu1_smp *smp, struct dpu1_smp_state *state,
		enum dpu1_pipe pipe, uint32_t blkcfg)
{
	struct dpu1_kms *dpu1_kms = get_kms(smp);
	struct drm_device *dev = dpu1_kms->dev;
	int i, ret;

	for (i = 0; i < pipe2nclients(pipe); i++) {
		u32 cid = pipe2client(pipe, i);
		int n = blkcfg & 0xff;

		if (!n)
			continue;

		DBG("%s[%d]: request %d SMP blocks", pipe2name(pipe), i, n);
		ret = smp_request_block(smp, state, cid, n);
		if (ret) {
			DRM_DEV_ERROR(dev->dev, "Cannot allocate %d SMP blocks: %d\n",
					n, ret);
			return ret;
		}

		blkcfg >>= 8;
	}

	state->assigned |= (1 << pipe);

	return 0;
}

/* Release SMP blocks for all clients of the pipe */
void dpu1_smp_release(struct dpu1_smp *smp, struct dpu1_smp_state *state,
		enum dpu1_pipe pipe)
{
	int i;
	int cnt = smp->blk_cnt;

	for (i = 0; i < pipe2nclients(pipe); i++) {
		u32 cid = pipe2client(pipe, i);
		void *cs = state->client_state[cid];

		/* update global state: */
		bitmap_andnot(state->state, state->state, cs, cnt);

		/* clear client's state */
		bitmap_zero(cs, cnt);
	}

	state->released |= (1 << pipe);
}

/* NOTE: SMP_ALLOC_* regs are *not* double buffered, so release has to
 * happen after scanout completes.
 */
static unsigned update_smp_state(struct dpu1_smp *smp,
		u32 cid, dpu1_smp_state_t *assigned)
{
	int cnt = smp->blk_cnt;
	unsigned nblks = 0;
	u32 blk, val;

	for_each_set_bit(blk, *assigned, cnt) {
		int idx = blk / 3;
		int fld = blk % 3;

		val = smp->alloc_w[idx];

		switch (fld) {
		case 0:
			val &= ~MDP5_SMP_ALLOC_W_REG_CLIENT0__MASK;
			val |= MDP5_SMP_ALLOC_W_REG_CLIENT0(cid);
			break;
		case 1:
			val &= ~MDP5_SMP_ALLOC_W_REG_CLIENT1__MASK;
			val |= MDP5_SMP_ALLOC_W_REG_CLIENT1(cid);
			break;
		case 2:
			val &= ~MDP5_SMP_ALLOC_W_REG_CLIENT2__MASK;
			val |= MDP5_SMP_ALLOC_W_REG_CLIENT2(cid);
			break;
		}

		smp->alloc_w[idx] = val;
		smp->alloc_r[idx] = val;

		nblks++;
	}

	return nblks;
}

static void write_smp_alloc_regs(struct dpu1_smp *smp)
{
	struct dpu1_kms *dpu1_kms = get_kms(smp);
	int i, num_regs;

	num_regs = smp->blk_cnt / 3 + 1;

	for (i = 0; i < num_regs; i++) {
		dpu1_write(dpu1_kms, REG_MDP5_SMP_ALLOC_W_REG(i),
			   smp->alloc_w[i]);
		dpu1_write(dpu1_kms, REG_MDP5_SMP_ALLOC_R_REG(i),
			   smp->alloc_r[i]);
	}
}

static void write_smp_fifo_regs(struct dpu1_smp *smp)
{
	struct dpu1_kms *dpu1_kms = get_kms(smp);
	int i;

	for (i = 0; i < dpu1_kms->num_hwpipes; i++) {
		struct dpu1_hw_pipe *hwpipe = dpu1_kms->hwpipes[i];
		enum dpu1_pipe pipe = hwpipe->pipe;

		dpu1_write(dpu1_kms, REG_MDP5_PIPE_REQPRIO_FIFO_WM_0(pipe),
			   smp->pipe_reqprio_fifo_wm0[pipe]);
		dpu1_write(dpu1_kms, REG_MDP5_PIPE_REQPRIO_FIFO_WM_1(pipe),
			   smp->pipe_reqprio_fifo_wm1[pipe]);
		dpu1_write(dpu1_kms, REG_MDP5_PIPE_REQPRIO_FIFO_WM_2(pipe),
			   smp->pipe_reqprio_fifo_wm2[pipe]);
	}
}

void dpu1_smp_prepare_commit(struct dpu1_smp *smp, struct dpu1_smp_state *state)
{
	enum dpu1_pipe pipe;

	for_each_set_bit(pipe, &state->assigned, sizeof(state->assigned) * 8) {
		unsigned i, nblks = 0;

		for (i = 0; i < pipe2nclients(pipe); i++) {
			u32 cid = pipe2client(pipe, i);
			void *cs = state->client_state[cid];

			nblks += update_smp_state(smp, cid, cs);

			DBG("assign %s:%u, %u blks",
				pipe2name(pipe), i, nblks);
		}

		set_fifo_thresholds(smp, pipe, nblks);
	}

	write_smp_alloc_regs(smp);
	write_smp_fifo_regs(smp);

	state->assigned = 0;
}

void dpu1_smp_complete_commit(struct dpu1_smp *smp, struct dpu1_smp_state *state)
{
	enum dpu1_pipe pipe;

	for_each_set_bit(pipe, &state->released, sizeof(state->released) * 8) {
		DBG("release %s", pipe2name(pipe));
		set_fifo_thresholds(smp, pipe, 0);
	}

	write_smp_fifo_regs(smp);

	state->released = 0;
}

void dpu1_smp_dump(struct dpu1_smp *smp, struct drm_printer *p,
		   struct dpu1_global_state *global_state)
{
	struct dpu1_kms *dpu1_kms = get_kms(smp);
	struct dpu1_hw_pipe_state *hwpstate;
	struct dpu1_smp_state *state;
	int total = 0, i, j;

	drm_printf(p, "name\tinuse\tplane\n");
	drm_printf(p, "----\t-----\t-----\n");

	/* grab these *after* we hold the state_lock */
	hwpstate = &global_state->hwpipe;
	state = &global_state->smp;

	for (i = 0; i < dpu1_kms->num_hwpipes; i++) {
		struct dpu1_hw_pipe *hwpipe = dpu1_kms->hwpipes[i];
		struct drm_plane *plane = hwpstate->hwpipe_to_plane[hwpipe->idx];
		enum dpu1_pipe pipe = hwpipe->pipe;
		for (j = 0; j < pipe2nclients(pipe); j++) {
			u32 cid = pipe2client(pipe, j);
			void *cs = state->client_state[cid];
			int inuse = bitmap_weight(cs, smp->blk_cnt);

			drm_printf(p, "%s:%d\t%d\t%s\n",
				pipe2name(pipe), j, inuse,
				plane ? plane->name : NULL);

			total += inuse;
		}
	}

	drm_printf(p, "TOTAL:\t%d\t(of %d)\n", total, smp->blk_cnt);
	drm_printf(p, "AVAIL:\t%d\n", smp->blk_cnt -
			bitmap_weight(state->state, smp->blk_cnt));
}


struct dpu1_smp *dpu1_smp_init(struct dpu1_kms *dpu1_kms, const struct dpu1_smp_block *cfg)
{
	struct drm_device *dev = dpu1_kms->dev;
	struct dpu1_smp_state *state;
	struct dpu1_global_state *global_state;
	struct dpu1_smp *smp;

	smp = devm_kzalloc(dev->dev, sizeof(*smp), GFP_KERNEL);
	if (unlikely(!smp))
		return ERR_PTR(-ENOMEM);

	smp->dev = dpu1_kms->dev;
	smp->blk_cnt = cfg->mmb_count;
	smp->blk_size = cfg->mmb_size;

	global_state = dpu1_get_existing_global_state(dpu1_kms);
	state = &global_state->smp;

	/* statically tied MMBs cannot be re-allocated: */
	bitmap_copy(state->state, cfg->reserved_state, smp->blk_cnt);
	memcpy(smp->reserved, cfg->reserved, sizeof(smp->reserved));

	return smp;
}
