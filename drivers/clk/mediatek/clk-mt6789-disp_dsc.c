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

static const struct mtk_gate_regs disp_dsc_cg_regs = {
	.set_ofs = 0x0,
	.clr_ofs = 0x0,
	.sta_ofs = 0x0,
};

#define GATE_DISP_DSC(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &disp_dsc_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_no_setclr_inv,	\
	}

static const struct mtk_gate disp_dsc_clks[] = {
	GATE_DISP_DSC(CLK_DISP_DSC_DSC_EN, "disp_dsc_dsc_en", "disp_ck", 0),
};

static const struct mtk_clk_desc disp_dsc_mcd = {
	.clks = disp_dsc_clks,
	.num_clks = ARRAY_SIZE(disp_dsc_clks),
};

static const struct of_device_id of_match_clk_mt6789_disp_dsc[] = {
	{
		.compatible = "mediatek,mt6789-disp_dsc",
		.data = &disp_dsc_mcd,
	},
	{}
};

MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_disp_dsc);

static struct platform_driver clk_mt6789_disp_dsc_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-disp_dsc",
		.of_match_table = of_match_clk_mt6789_disp_dsc,
	},
};

module_platform_driver(clk_mt6789_disp_dsc_drv);
MODULE_LICENSE("GPL");
