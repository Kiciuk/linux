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

static const struct mtk_gate_regs impc_cg_regs = {
	.set_ofs = 0xE08,
	.clr_ofs = 0xE04,
	.sta_ofs = 0xE00,
};

#define GATE_IMPC(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &impc_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate impc_clks[] = {
	GATE_IMPC(CLK_IMPC_AP_CLOCK_I2C3, "impc_ap_clock_i2c3", "i2c_pseudo_ck", 0),
	GATE_IMPC(CLK_IMPC_AP_CLOCK_I2C5, "impc_ap_clock_i2c5",	"i2c_pseudo_ck", 1),
	GATE_IMPC(CLK_IMPC_AP_CLOCK_I2C6, "impc_ap_clock_i2c6",	"i2c_pseudo_ck", 2),
};

static const struct mtk_clk_desc impc_mcd = {
	.clks = impc_clks,
	.num_clks = ARRAY_SIZE(impc_clks),
};

static const struct mtk_gate_regs impn_cg_regs = {
	.set_ofs = 0xE08,
	.clr_ofs = 0xE04,
	.sta_ofs = 0xE00,
};

#define GATE_IMPN(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &impn_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate impn_clks[] = {
	GATE_IMPN(CLK_IMPN_AP_CLOCK_I2C7, "impn_ap_clock_i2c7",	"i2c_pseudo_ck", 0),
};

static const struct mtk_clk_desc impn_mcd = {
	.clks = impn_clks,
	.num_clks = ARRAY_SIZE(impn_clks),
};

static const struct mtk_gate_regs impw_cg_regs = {
	.set_ofs = 0xE08,
	.clr_ofs = 0xE04,
	.sta_ofs = 0xE00,
};

#define GATE_IMPW(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &impw_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate impw_clks[] = {
	GATE_IMPW(CLK_IMPW_AP_CLOCK_I2C0, "impw_ap_clock_i2c0",	"i2c_pseudo_ck", 0),
	GATE_IMPW(CLK_IMPW_AP_CLOCK_I2C1, "impw_ap_clock_i2c1",	"i2c_pseudo_ck", 1),
};

static const struct mtk_clk_desc impw_mcd = {
	.clks = impw_clks,
	.num_clks = ARRAY_SIZE(impw_clks),
};

static const struct of_device_id of_match_clk_mt6789_i2c[] = {
	{
		.compatible = "mediatek,mt6789-imp_iic_wrap_c",
		.data = &impc_mcd,
	}, {
		.compatible = "mediatek,mt6789-imp_iic_wrap_n",
		.data = &impn_mcd,
	}, {
		.compatible = "mediatek,mt6789-imp_iic_wrap_w",
		.data = &impw_mcd,
	}, {
		/* sentinel */
	}
};

MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_i2c);

static struct platform_driver clk_mt6789_i2c_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-i2c",
		.of_match_table = of_match_clk_mt6789_i2c,
	},
};

module_platform_driver(clk_mt6789_i2c_drv);
MODULE_LICENSE("GPL");
