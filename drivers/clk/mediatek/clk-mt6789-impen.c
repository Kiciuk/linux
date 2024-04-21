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

static const struct mtk_gate_regs impen_cg_regs = {
	.set_ofs = 0xE08,
	.clr_ofs = 0xE04,
	.sta_ofs = 0xE00,
};

#define GATE_IMPEN(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &impen_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate impen_clks[] = {
	GATE_IMPEN(CLK_IMPEN_AP_CLOCK_I2C2, "impen_ap_clock_i2c2", "i2c_pseudo_ck", 0),
	GATE_IMPEN(CLK_IMPEN_AP_CLOCK_I2C4, "impen_ap_clock_i2c4", "i2c_pseudo_ck", 1),
	GATE_IMPEN(CLK_IMPEN_AP_CLOCK_I2C8, "impen_ap_clock_i2c8", "i2c_pseudo_ck", 2),
	GATE_IMPEN(CLK_IMPEN_AP_CLOCK_I2C9, "impen_ap_clock_i2c9", "i2c_pseudo_ck", 3),
};

static const struct mtk_clk_desc impen_mcd = {
	.clks = impen_clks,
	.num_clks = ARRAY_SIZE(impen_clks),
};

static const struct of_device_id of_match_clk_mt6789_impen[] = {
	{
		.compatible = "mediatek,mt6789-imp_iic_wrap_en",
		.data = &impen_mcd,
	},
	{}
};

MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_impen);
static struct platform_driver clk_mt6789_impen_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-impen",
		.of_match_table = of_match_clk_mt6789_impen,
	},
};

module_platform_driver(clk_mt6789_impen_drv);
MODULE_LICENSE("GPL");
