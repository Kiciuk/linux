/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#ifndef __DPU1_SMP_H__
#define __DPU1_SMP_H__

#include <drm/drm_print.h>

#include "msm_drv.h"

/*
 * SMP - Shared Memory Pool:
 *
 * SMP blocks are shared between all the clients, where each plane in
 * a scanout buffer is a SMP client.  Ie. scanout of 3 plane I420 on
 * pipe VIG0 => 3 clients: VIG0_Y, VIG0_CB, VIG0_CR.
 *
 * Based on the size of the attached scanout buffer, a certain # of
 * blocks must be allocated to that client out of the shared pool.
 *
 * In some hw, some blocks are statically allocated for certain pipes
 * and CANNOT be re-allocated (eg: MMB0 and MMB1 both tied to RGB0).
 *
 *
 * Atomic SMP State:
 *
 * On atomic updates that modify SMP configuration, the state is cloned
 * (copied) and modified.  For test-only, or in cases where atomic
 * update fails (or if we hit ww_mutex deadlock/backoff condition) the
 * new state is simply thrown away.
 *
 * Because the SMP registers are not double buffered, updates are a
 * two step process:
 *
 * 1) in _prepare_commit() we configure things (via read-modify-write)
 *    for the newly assigned pipes, so we don't take away blocks
 *    assigned to pipes that are still scanning out
 * 2) in _complete_commit(), after vblank/etc, we clear things for the
 *    released clients, since at that point old pipes are no longer
 *    scanning out.
 */
struct dpu1_smp_state {
	/* global state of what blocks are in use: */
	dpu1_smp_state_t state;

	/* per client state of what blocks they are using: */
	dpu1_smp_state_t client_state[MAX_CLIENTS];

	/* assigned pipes (hw updated at _prepare_commit()): */
	unsigned long assigned;

	/* released pipes (hw updated at _complete_commit()): */
	unsigned long released;
};

struct dpu1_kms;
struct dpu1_smp;

/*
 * SMP module prototypes:
 * dpu1_smp_init() returns a SMP @handler,
 * which is then used to call the other dpu1_smp_*(handler, ...) functions.
 */

struct dpu1_smp *dpu1_smp_init(struct dpu1_kms *dpu1_kms,
		const struct dpu1_smp_block *cfg);

struct dpu1_global_state;
void dpu1_smp_dump(struct dpu1_smp *smp, struct drm_printer *p,
		   struct dpu1_global_state *global_state);

uint32_t dpu1_smp_calculate(struct dpu1_smp *smp,
		const struct mdp_format *format,
		u32 width, bool hdecim);

int dpu1_smp_assign(struct dpu1_smp *smp, struct dpu1_smp_state *state,
		enum dpu1_pipe pipe, uint32_t blkcfg);
void dpu1_smp_release(struct dpu1_smp *smp, struct dpu1_smp_state *state,
		enum dpu1_pipe pipe);

void dpu1_smp_prepare_commit(struct dpu1_smp *smp, struct dpu1_smp_state *state);
void dpu1_smp_complete_commit(struct dpu1_smp *smp, struct dpu1_smp_state *state);

#endif /* __DPU1_SMP_H__ */
