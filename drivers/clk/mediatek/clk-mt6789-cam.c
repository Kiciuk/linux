// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mediatek,mt6789-clk.h>

static const struct mtk_gate_regs cam_m_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

#define GATE_CAM_M(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &cam_m_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate cam_m_clks[] = {
	GATE_CAM_M(CLK_CAM_M_LARB13, "cam_m_larb13", "cam_ck", 0),
	GATE_CAM_M(CLK_CAM_M_LARB14, "cam_m_larb14", "cam_ck", 2),
	GATE_CAM_M(CLK_CAM_M_CAM, "cam_m_cam", "cam_ck", 6),
	GATE_CAM_M(CLK_CAM_M_CAMTG, "cam_m_camtg", "cam_ck", 7),
	GATE_CAM_M(CLK_CAM_M_SENINF, "cam_m_seninf",  "cam_ck", 8),
	GATE_CAM_M(CLK_CAM_M_CAMSV1, "cam_m_camsv1", "cam_ck", 10),
	GATE_CAM_M(CLK_CAM_M_CAMSV2, "cam_m_camsv2", "cam_ck", 11),
	GATE_CAM_M(CLK_CAM_M_CAMSV3, "cam_m_camsv3", "cam_ck", 12),
	GATE_CAM_M(CLK_CAM_M_MRAW0, "cam_m_mraw0", "cam_ck", 15),
	GATE_CAM_M(CLK_CAM_M_FAKE_ENG, "cam_m_fake_eng", "cam_ck", 17),
	GATE_CAM_M(CLK_CAM_M_CAM2MM_GALS, "cam_m_cam2mm_gals", "cam_ck", 19),
};

static const struct mtk_clk_desc cam_m_mcd = {
	.clks = cam_m_clks,
	.num_clks = ARRAY_SIZE(cam_m_clks),
};

static const struct mtk_gate_regs cam_ra_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

#define GATE_CAM_RA(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &cam_ra_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate cam_ra_clks[] = {
	GATE_CAM_RA(CLK_CAM_RA_LARBX, "cam_ra_larbx", "cam_ck", 0),
	GATE_CAM_RA(CLK_CAM_RA_CAM, "cam_ra_cam", "cam_ck", 1),
	GATE_CAM_RA(CLK_CAM_RA_CAMTG, "cam_ra_camtg", "camtm_ck", 2),
};

static const struct mtk_clk_desc cam_ra_mcd = {
	.clks = cam_ra_clks,
	.num_clks = ARRAY_SIZE(cam_ra_clks),
};

static const struct mtk_gate_regs cam_rb_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

#define GATE_CAM_RB(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &cam_rb_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate cam_rb_clks[] = {
	GATE_CAM_RB(CLK_CAM_RB_LARBX, "cam_rb_larbx", "cam_ck", 0),
	GATE_CAM_RB(CLK_CAM_RB_CAM, "cam_rb_cam", "cam_ck", 1),
	GATE_CAM_RB(CLK_CAM_RB_CAMTG, "cam_rb_camtg", "camtm_ck", 2),
};

static const struct mtk_clk_desc cam_rb_mcd = {
	.clks = cam_rb_clks,
	.num_clks = ARRAY_SIZE(cam_rb_clks),
};

static const struct of_device_id of_match_clk_mt6789_cam[] = {
	{
		.compatible = "mediatek,mt6789-camsys_main",
		.data = &cam_m_mcd,
	}, {
		.compatible = "mediatek,mt6789-camsys_rawa",
		.data = &cam_ra_mcd,
	}, {
		.compatible = "mediatek,mt6789-camsys_rawb",
		.data = &cam_rb_mcd,
	}, {
		/* sentinel */
	}
};

MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_cam);

static struct platform_driver clk_mt6789_cam_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-cam",
		.of_match_table = of_match_clk_mt6789_cam,
	},
};

module_platform_driver(clk_mt6789_cam_drv);
MODULE_LICENSE("GPL");
