// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2020, The Linux Foundation. All rights reserved.

#include <linux/bitops.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

#include <dt-bindings/clock/qcom,gcc-msm8953.h>

#include "clk-alpha-pll.h"
#include "clk-branch.h"
#include "clk-pll.h"
#include "clk-rcg.h"
#include "common.h"
#include "gdsc.h"
#include "reset.h"

enum {
	P_XO,
	P_GPLL0,
	P_GPLL0_DIV2,
	P_GPLL0_DIV2_CCI,
	P_GPLL0_DIV2_MM,
	P_GPLL0_DIV2_USB3,
	P_GPLL2,
	P_GPLL3,
	P_GPLL4,
	P_GPLL6,
	P_GPLL6_DIV2,
	P_GPLL6_DIV2_GFX,
	P_GPLL6_DIV2_MOCK,
	P_DSI0PLL,
	P_DSI1PLL,
	P_DSI0PLL_BYTE,
	P_DSI1PLL_BYTE,
};

static struct clk_alpha_pll gpll0_early = {
	.offset = 0x21000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr = {
		.enable_reg = 0x45000,
		.enable_mask = BIT(0),
		.hw.init = &(struct clk_init_data) {
			.num_parents = 1,
			.parent_data = &(const struct clk_parent_data) {
				.fw_name = "xo", .name = "xo",
			},
			.name = "gpll0_early",
			.ops = &clk_alpha_pll_fixed_ops,
		},
	},
};

static struct clk_fixed_factor gpll0_early_div = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gpll0_early_div",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll0_early.clkr.hw,
		},
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_alpha_pll_postdiv gpll0 = {
	.offset = 0x21000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll0",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll0_early.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ops,
	},
};

static struct clk_alpha_pll gpll2_early = {
	.offset = 0x4A000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr = {
		.enable_reg = 0x45000,
		.enable_mask = BIT(2),
		.hw.init = &(struct clk_init_data){
			.num_parents = 1,
			.parent_data = &(const struct clk_parent_data) {
				.fw_name = "xo", .name = "xo",
			},
			.name = "gpll2_early",
			.ops = &clk_alpha_pll_fixed_ops,
		},
	},
};

static struct clk_alpha_pll_postdiv gpll2 = {
	.offset = 0x4A000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll2",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll2_early.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ops,
	},
};

static struct pll_vco gpll3_p_vco[] = {
	{ 1000000000, 2000000000, 0 },
};

static struct clk_alpha_pll gpll3_early = {
	.offset = 0x22000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.vco_table = gpll3_p_vco,
	.num_vco = ARRAY_SIZE(gpll3_p_vco),
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.num_parents = 1,
			.parent_data = &(const struct clk_parent_data) {
				.fw_name = "xo", .name = "xo",
			},
			.name = "gpll3_early",
			.ops = &clk_alpha_pll_ops,
		},
	},
};

static struct clk_fixed_factor gpll3_early_div = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gpll3_early_div",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll3_early.clkr.hw,
		},
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_alpha_pll_postdiv gpll3 = {
	.offset = 0x22000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll3",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll3_early.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ops,
	},
};

static struct clk_alpha_pll gpll4_early = {
	.offset = 0x24000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr = {
		.enable_reg = 0x45000,
		.enable_mask = BIT(5),
		.hw.init = &(struct clk_init_data){
			.num_parents = 1,
			.parent_data = &(const struct clk_parent_data) {
				.fw_name = "xo", .name = "xo",
			},
			.name = "gpll4_early",
			.ops = &clk_alpha_pll_fixed_ops,
		},
	},
};

static struct clk_alpha_pll_postdiv gpll4 = {
	.offset = 0x24000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll4",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll4_early.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ops,
	},
};

static struct clk_alpha_pll gpll6_early = {
	.offset = 0x37000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr = {
		.enable_reg = 0x45000,
		.enable_mask = BIT(7),
		.hw.init = &(struct clk_init_data){
			.num_parents = 1,
			.parent_data = &(const struct clk_parent_data) {
				.fw_name = "xo", .name = "xo",
			},
			.name = "gpll6_early",
			.ops = &clk_alpha_pll_fixed_ops,
		},
	},
};

static struct clk_fixed_factor gpll6_early_div = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gpll6_early_div",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll6_early.clkr.hw,
		},
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_alpha_pll_postdiv gpll6 = {
	.offset = 0x37000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_PLL_TYPE_DEFAULT],
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll6",
		.parent_data = &(const struct clk_parent_data) {
			.hw = &gpll6_early.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ops,
	},
};

static const struct parent_map xo_g0_g4_g0d_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
	{ P_GPLL4, 2 },
	{ P_GPLL0_DIV2, 4 },
};

static const struct clk_parent_data xo_g0_g4_g0d_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
};

static const struct parent_map g0_g0d_g2_map[] = {
	{ P_GPLL0, 1 },
	{ P_GPLL0_DIV2, 4 },
	{ P_GPLL2, 5 },
};

static const struct clk_parent_data g0_g0d_g2_data[] = {
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll2.clkr.hw },
};

static const struct parent_map g0_g0d_g2_g0d_map[] = {
	{ P_GPLL0, 1 },
	{ P_GPLL0_DIV2_USB3, 2 },
	{ P_GPLL2, 4 },
	{ P_GPLL0_DIV2_MM, 5 },
};

static const struct clk_parent_data g0_g0d_g2_g0d_data[] = {
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
};

static const struct parent_map xo_g0_g6d_g0d_g4_g0d_g6d_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
	{ P_GPLL6_DIV2_MOCK, 2 },
	{ P_GPLL0_DIV2_CCI, 3 },
	{ P_GPLL4, 4 },
	{ P_GPLL0_DIV2_MM, 5 },
	{ P_GPLL6_DIV2_GFX, 6 },
};

static const struct clk_parent_data xo_g0_g6d_g0d_g4_g0d_g6d_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll6_early_div.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll6_early_div.hw },
};

static const struct parent_map g0_g6_g2_g0d_g6d_map[] = {
	{ P_GPLL0, 1 },
	{ P_GPLL6, 2 },
	{ P_GPLL2, 3 },
	{ P_GPLL0_DIV2, 4 },
	{ P_GPLL6_DIV2, 5 },
};

static const struct clk_parent_data g0_g6_g2_g0d_g6d_data[] = {
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll6.clkr.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll6_early_div.hw },
};

static const struct parent_map xo_dsi0pll_dsi1pll_map[] = {
	{ P_XO, 0 },
	{ P_DSI0PLL, 1 },
	{ P_DSI1PLL, 3 },
};

static const struct clk_parent_data xo_dsi0pll_dsi1pll_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .fw_name = "dsi0pll", .name = "dsi0pll" },
	{ .fw_name = "dsi1pll", .name = "dsi1pll" },
};

static const struct parent_map xo_dsi1pll_dsi0pll_map[] = {
	{ P_XO, 0 },
	{ P_DSI1PLL, 1 },
	{ P_DSI0PLL, 3 },
};

static const struct clk_parent_data xo_dsi1pll_dsi0pll_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .fw_name = "dsi1pll", .name = "dsi1pll" },
	{ .fw_name = "dsi0pll", .name = "dsi0pll" },
};

static const struct parent_map xo_dsi0pllbyte_dsi1pllbyte_map[] = {
	{ P_XO, 0 },
	{ P_DSI0PLL_BYTE, 1 },
	{ P_DSI1PLL_BYTE, 3 },
};

static const struct clk_parent_data xo_dsi0pllbyte_dsi1pllbyte_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .fw_name = "dsi0pllbyte", .name = "dsi0pllbyte" },
	{ .fw_name = "dsi1pllbyte", .name = "dsi1pllbyte" },
};

static const struct parent_map xo_dsi1pllbyte_dsi0pllbyte_map[] = {
	{ P_XO, 0 },
	{ P_DSI1PLL_BYTE, 1 },
	{ P_DSI0PLL_BYTE, 3 },
};

static const struct clk_parent_data xo_dsi1pllbyte_dsi0pllbyte_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .fw_name = "dsi1pllbyte", .name = "dsi1pllbyte" },
	{ .fw_name = "dsi0pllbyte", .name = "dsi0pllbyte" },
};

static const struct parent_map gfx3d_map[] = {
	{ P_XO, 0 },
	{ P_GPLL0, 1 },
	{ P_GPLL3, 2 },
	{ P_GPLL4, 4 },
	{ P_GPLL0_DIV2_MM, 5 },
	{ P_GPLL6_DIV2_GFX, 6 },
};

static const struct clk_parent_data gfx3d_data[] = {
	{ .fw_name = "xo", .name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll3.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0_early_div.hw },
	{ .hw = &gpll6_early_div.hw },
};

static struct freq_tbl ftbl_camss_top_ahb_clk_src[] = {
	F(40000000, P_GPLL0_DIV2, 10, 0, 0),
	F(80000000, P_GPLL0, 10, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi0_clk_src[] = {
	F(100000000, P_GPLL0_DIV2_MM, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_apss_ahb_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(25000000, P_GPLL0_DIV2, 16, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi1_clk_src[] = {
	F(100000000, P_GPLL0_DIV2, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi2_clk_src[] = {
	F(100000000, P_GPLL0_DIV2, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_vfe0_clk_src[] = {
	F(50000000, P_GPLL0_DIV2_MM, 8, 0, 0),
	F(100000000, P_GPLL0_DIV2_MM, 4, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_gfx3d_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(128000000, P_GPLL4, 9, 0, 0),
	F(230400000, P_GPLL4, 5, 0, 0),
	F(384000000, P_GPLL4, 3, 0, 0),
	F(460800000, P_GPLL4, 2.5, 0, 0),
	F(576000000, P_GPLL4, 2, 0, 0),
	F(652800000, P_GPLL3, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_vcodec0_clk_src[] = {
	F(114290000, P_GPLL0_DIV2, 3.5, 0, 0),
	F(228570000, P_GPLL0, 3.5, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(360000000, P_GPLL6, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_cpp_clk_src[] = {
	F(100000000, P_GPLL0_DIV2_MM, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(320000000, P_GPLL0, 2.5, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_jpeg0_clk_src[] = {
	F(66670000, P_GPLL0_DIV2, 6, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(320000000, P_GPLL0, 2.5, 0, 0),
	{ }
};

static struct freq_tbl ftbl_mdp_clk_src[] = {
	F(50000000, P_GPLL0_DIV2, 8, 0, 0),
	F(80000000, P_GPLL0_DIV2, 5, 0, 0),
	F(160000000, P_GPLL0_DIV2, 2.5, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(320000000, P_GPLL0, 2.5, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_usb30_master_clk_src[] = {
	F(80000000, P_GPLL0_DIV2_USB3, 5, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	{ }
};

static struct freq_tbl ftbl_vfe1_clk_src[] = {
	F(50000000, P_GPLL0_DIV2_MM, 8, 0, 0),
	F(100000000, P_GPLL0_DIV2_MM, 4, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(465000000, P_GPLL2, 2, 0, 0),
	{ }
};

static struct freq_tbl ftbl_apc0_droop_detector_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(576000000, P_GPLL4, 2, 0, 0),
	{ }
};


static struct freq_tbl ftbl_apc1_droop_detector_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(400000000, P_GPLL0, 2, 0, 0),
	F(576000000, P_GPLL4, 2, 0, 0),
	{ }
};


static struct freq_tbl ftbl_blsp_i2c_apps_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(25000000, P_GPLL0_DIV2, 16, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	{ }
};

static struct freq_tbl ftbl_blsp_spi_apps_clk_src[] = {
	F(960000, P_XO, 10, 1, 2),
	F(4800000, P_XO, 4, 0, 0),
	F(9600000, P_XO, 2, 0, 0),
	F(12500000, P_GPLL0_DIV2, 16, 1, 2),
	F(16000000, P_GPLL0, 10, 1, 5),
	F(19200000, P_XO, 1, 0, 0),
	F(25000000, P_GPLL0, 16, 1, 2),
	F(50000000, P_GPLL0, 16, 0, 0),
	{ }
};

static struct freq_tbl ftbl_blsp_uart_apps_clk_src[] = {
	F(3686400, P_GPLL0_DIV2, 1, 144, 15625),
	F(7372800, P_GPLL0_DIV2, 1, 288, 15625),
	F(14745600, P_GPLL0_DIV2, 1, 576, 15625),
	F(16000000, P_GPLL0_DIV2, 5, 1, 5),
	F(19200000, P_XO, 1, 0, 0),
	F(24000000, P_GPLL0, 1, 3, 100),
	F(25000000, P_GPLL0, 16, 1, 2),
	F(32000000, P_GPLL0, 1, 1, 25),
	F(40000000, P_GPLL0, 1, 1, 20),
	F(46400000, P_GPLL0, 1, 29, 500),
	F(48000000, P_GPLL0, 1, 3, 50),
	F(51200000, P_GPLL0, 1, 8, 125),
	F(56000000, P_GPLL0, 1, 7, 100),
	F(58982400, P_GPLL0, 1, 1152, 15625),
	F(60000000, P_GPLL0, 1, 3, 40),
	F(64000000, P_GPLL0, 1, 2, 25),
	{ }
};

static struct freq_tbl ftbl_cci_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(37500000, P_GPLL0_DIV2_CCI, 1, 3, 32),
	{ }
};

static struct freq_tbl ftbl_csi0p_clk_src[] = {
	F(66670000, P_GPLL0_DIV2_MM, 6, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi1p_clk_src[] = {
	F(66670000, P_GPLL0_DIV2_MM, 6, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi2p_clk_src[] = {
	F(66670000, P_GPLL0_DIV2_MM, 6, 0, 0),
	F(133330000, P_GPLL0, 6, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	F(310000000, P_GPLL2, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_camss_gp0_clk_src[] = {
	F(50000000, P_GPLL0_DIV2, 8, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_camss_gp1_clk_src[] = {
	F(50000000, P_GPLL0_DIV2, 8, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_mclk0_clk_src[] = {
	F(24000000, P_GPLL6_DIV2, 1, 2, 45),
	F(33330000, P_GPLL0_DIV2, 12, 0, 0),
	F(36610000, P_GPLL6, 1, 2, 59),
	F(66667000, P_GPLL0, 12, 0, 0),
	{ }
};

static struct freq_tbl ftbl_mclk1_clk_src[] = {
	F(24000000, P_GPLL6_DIV2, 1, 2, 45),
	F(33330000, P_GPLL0_DIV2, 12, 0, 0),
	F(36610000, P_GPLL6, 1, 2, 59),
	F(66667000, P_GPLL0, 12, 0, 0),
	{ }
};

static struct freq_tbl ftbl_mclk2_clk_src[] = {
	F(24000000, P_GPLL6_DIV2, 1, 2, 45),
	F(33330000, P_GPLL0_DIV2, 12, 0, 0),
	F(36610000, P_GPLL6, 1, 2, 59),
	F(66667000, P_GPLL0, 12, 0, 0),
	{ }
};

static struct freq_tbl ftbl_mclk3_clk_src[] = {
	F(24000000, P_GPLL6_DIV2, 1, 2, 45),
	F(33330000, P_GPLL0_DIV2, 12, 0, 0),
	F(36610000, P_GPLL6, 1, 2, 59),
	F(66667000, P_GPLL0, 12, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi0phytimer_clk_src[] = {
	F(100000000, P_GPLL0_DIV2, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi1phytimer_clk_src[] = {
	F(100000000, P_GPLL0_DIV2, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_csi2phytimer_clk_src[] = {
	F(100000000, P_GPLL0_DIV2, 4, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	F(266670000, P_GPLL0, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_crypto_clk_src[] = {
	F(40000000, P_GPLL0_DIV2, 10, 0, 0),
	F(80000000, P_GPLL0, 10, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	{ }
};

static struct freq_tbl ftbl_gp1_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

static struct freq_tbl ftbl_gp2_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

static struct freq_tbl ftbl_gp3_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

static struct freq_tbl ftbl_esc0_1_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

static struct freq_tbl ftbl_vsync_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

static struct freq_tbl ftbl_pdm2_clk_src[] = {
	F(32000000, P_GPLL0_DIV2, 12.5, 0, 0),
	F(64000000, P_GPLL0, 12.5, 0, 0),
	{ }
};

static struct freq_tbl ftbl_rbcpr_gfx_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	{ }
};

static struct freq_tbl ftbl_sdcc1_apps_clk_src[] = {
	F(144000, P_XO, 16, 3, 25),
	F(400000, P_XO, 12, 1, 4),
	F(20000000, P_GPLL0_DIV2, 5, 1, 4),
	F(25000000, P_GPLL0_DIV2, 16, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(177770000, P_GPLL0, 4.5, 0, 0),
	F(192000000, P_GPLL4, 6, 0, 0),
	F(384000000, P_GPLL4, 3, 0, 0),
	{ }
};

static struct freq_tbl ftbl_sdcc1_ice_core_clk_src[] = {
	F(80000000, P_GPLL0_DIV2, 5, 0, 0),
	F(160000000, P_GPLL0, 5, 0, 0),
	F(270000000, P_GPLL6, 4, 0, 0),
	{ }
};

static struct freq_tbl ftbl_sdcc2_apps_clk_src[] = {
	F(144000, P_XO, 16, 3, 25),
	F(400000, P_XO, 12, 1, 4),
	F(20000000, P_GPLL0_DIV2, 5, 1, 4),
	F(25000000, P_GPLL0_DIV2, 16, 0, 0),
	F(50000000, P_GPLL0, 16, 0, 0),
	F(100000000, P_GPLL0, 8, 0, 0),
	F(177770000, P_GPLL0, 4.5, 0, 0),
	F(192000000, P_GPLL4, 6, 0, 0),
	F(200000000, P_GPLL0, 4, 0, 0),
	{ }
};

static struct freq_tbl ftbl_usb30_mock_utmi_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	F(60000000, P_GPLL6_DIV2_MOCK, 9, 1, 1),
	{ }
};

static struct freq_tbl ftbl_usb3_aux_clk_src[] = {
	F(19200000, P_XO, 1, 0, 0),
	{ }
};

#define DEFINE_RCG_COMMON(_name, _parents, _ftbl,  _ops, _rcgr, _mnd, _flags)	\
	static struct clk_rcg2 _name = {						\
		.cmd_rcgr = _rcgr,						\
		.hid_width = 5,							\
		.mnd_width = _mnd,						\
		.freq_tbl = _ftbl,						\
		.parent_map = _parents##_map,					\
		.clkr.hw.init = &(struct clk_init_data) {			\
			.num_parents = ARRAY_SIZE(_parents##_data),		\
			.parent_data = _parents##_data,				\
			.name = #_name,						\
			.ops = &_ops,						\
			.flags = _flags,					\
		}								\
	}
#define DEFINE_RCG(_name, _parents, _ftbl, _rcgr)   \
	DEFINE_RCG_COMMON(_name, _parents, _ftbl, clk_rcg2_ops, _rcgr, 0, 0)

#define DEFINE_RCG_MND(_name, _parents, _ftbl, _rcgr, _mnd)   \
	DEFINE_RCG_COMMON(_name, _parents, _ftbl, clk_rcg2_ops, _rcgr, _mnd, 0)

DEFINE_RCG(apc0_droop_detector_clk_src, xo_g0_g4_g0d,
		ftbl_apc0_droop_detector_clk_src, 0x78008);
DEFINE_RCG(apc1_droop_detector_clk_src, xo_g0_g4_g0d,
		ftbl_apc1_droop_detector_clk_src, 0x79008);
DEFINE_RCG(apss_ahb_clk_src, xo_g0_g4_g0d, ftbl_apss_ahb_clk_src, 0x46000);
DEFINE_RCG(blsp1_qup1_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x0200C);
DEFINE_RCG(blsp1_qup2_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x03000);
DEFINE_RCG(blsp1_qup3_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x04000);
DEFINE_RCG(blsp1_qup4_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x05000);
DEFINE_RCG(blsp2_qup1_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x0C00C);
DEFINE_RCG(blsp2_qup2_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x0D000);
DEFINE_RCG(blsp2_qup3_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x0F000);
DEFINE_RCG(blsp2_qup4_i2c_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_i2c_apps_clk_src, 0x18000);
DEFINE_RCG(camss_top_ahb_clk_src, g0_g0d_g2, ftbl_camss_top_ahb_clk_src, 0x5A000);
DEFINE_RCG(cpp_clk_src, g0_g0d_g2_g0d, ftbl_cpp_clk_src, 0x58018);
DEFINE_RCG(crypto_clk_src, xo_g0_g4_g0d, ftbl_crypto_clk_src, 0x16004);
DEFINE_RCG(csi0_clk_src, g0_g0d_g2_g0d, ftbl_csi0_clk_src, 0x4E020);
DEFINE_RCG(csi0p_clk_src, g0_g0d_g2_g0d, ftbl_csi0p_clk_src, 0x58084);
DEFINE_RCG(csi0phytimer_clk_src, g0_g0d_g2, ftbl_csi0phytimer_clk_src, 0x4E000);
DEFINE_RCG(csi1_clk_src, g0_g0d_g2, ftbl_csi1_clk_src, 0x4F020);
DEFINE_RCG(csi1p_clk_src, g0_g0d_g2_g0d, ftbl_csi1p_clk_src, 0x58094);
DEFINE_RCG(csi1phytimer_clk_src, xo_g0_g4_g0d, ftbl_csi1phytimer_clk_src, 0x4F000);
DEFINE_RCG(csi2_clk_src, g0_g0d_g2, ftbl_csi2_clk_src, 0x3C020);
DEFINE_RCG(csi2p_clk_src, g0_g0d_g2_g0d, ftbl_csi2p_clk_src, 0x580A4);
DEFINE_RCG(csi2phytimer_clk_src, xo_g0_g4_g0d, ftbl_csi2phytimer_clk_src, 0x4F05C);
DEFINE_RCG(esc0_clk_src, xo_g0_g4_g0d, ftbl_esc0_1_clk_src, 0x4D05C);
DEFINE_RCG(esc1_clk_src, xo_g0_g4_g0d, ftbl_esc0_1_clk_src, 0x4D0A8);
DEFINE_RCG(gfx3d_clk_src, gfx3d, ftbl_gfx3d_clk_src, 0x59000);
DEFINE_RCG(jpeg0_clk_src, g0_g0d_g2, ftbl_jpeg0_clk_src, 0x57000);
DEFINE_RCG(mdp_clk_src, g0_g0d_g2, ftbl_mdp_clk_src, 0x4D014);
DEFINE_RCG(pdm2_clk_src, g0_g0d_g2, ftbl_pdm2_clk_src, 0x44010);
DEFINE_RCG(rbcpr_gfx_clk_src, xo_g0_g4_g0d, ftbl_rbcpr_gfx_clk_src, 0x3A00C);
DEFINE_RCG(sdcc1_ice_core_clk_src, g0_g6_g2_g0d_g6d, ftbl_sdcc1_ice_core_clk_src, 0x5D000);
DEFINE_RCG(usb30_master_clk_src, g0_g0d_g2_g0d, ftbl_usb30_master_clk_src, 0x3F00C);
DEFINE_RCG(vcodec0_clk_src, g0_g6_g2_g0d_g6d, ftbl_vcodec0_clk_src, 0x4C000);
DEFINE_RCG(vfe0_clk_src, g0_g0d_g2_g0d, ftbl_vfe0_clk_src, 0x58000);
DEFINE_RCG(vfe1_clk_src, g0_g0d_g2_g0d, ftbl_vfe1_clk_src, 0x58054);
DEFINE_RCG(vsync_clk_src, xo_g0_g4_g0d, ftbl_vsync_clk_src, 0x4D02C);
DEFINE_RCG_MND(blsp1_qup1_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x02024, 8);
DEFINE_RCG_MND(blsp1_qup2_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x03014, 8);
DEFINE_RCG_MND(blsp1_qup3_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x04024, 8);
DEFINE_RCG_MND(blsp1_qup4_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x05024, 8);
DEFINE_RCG_MND(blsp1_uart1_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_uart_apps_clk_src, 0x02044, 16);
DEFINE_RCG_MND(blsp1_uart2_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_uart_apps_clk_src, 0x03034, 16);
DEFINE_RCG_MND(blsp2_qup1_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x0C024, 8);
DEFINE_RCG_MND(blsp2_qup2_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x0D014, 8);
DEFINE_RCG_MND(blsp2_qup3_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x0F024, 8);
DEFINE_RCG_MND(blsp2_qup4_spi_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_spi_apps_clk_src, 0x18024, 8);
DEFINE_RCG_MND(blsp2_uart1_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_uart_apps_clk_src, 0x0C044, 16);
DEFINE_RCG_MND(blsp2_uart2_apps_clk_src, xo_g0_g4_g0d, ftbl_blsp_uart_apps_clk_src, 0x0D034, 16);
DEFINE_RCG_MND(camss_gp0_clk_src, g0_g0d_g2, ftbl_camss_gp0_clk_src, 0x54000, 8);
DEFINE_RCG_MND(camss_gp1_clk_src, g0_g0d_g2, ftbl_camss_gp1_clk_src, 0x55000, 8);
DEFINE_RCG_MND(cci_clk_src, xo_g0_g6d_g0d_g4_g0d_g6d, ftbl_cci_clk_src, 0x51000, 8);
DEFINE_RCG_MND(gp1_clk_src, xo_g0_g4_g0d, ftbl_gp1_clk_src, 0x08004, 16);
DEFINE_RCG_MND(gp2_clk_src, xo_g0_g4_g0d, ftbl_gp2_clk_src, 0x09004, 16);
DEFINE_RCG_MND(gp3_clk_src, xo_g0_g4_g0d, ftbl_gp3_clk_src, 0x0A004, 16);
DEFINE_RCG_MND(mclk0_clk_src, g0_g6_g2_g0d_g6d, ftbl_mclk0_clk_src, 0x52000, 8);
DEFINE_RCG_MND(mclk1_clk_src, g0_g6_g2_g0d_g6d, ftbl_mclk1_clk_src, 0x53000, 8);
DEFINE_RCG_MND(mclk2_clk_src, g0_g6_g2_g0d_g6d, ftbl_mclk2_clk_src, 0x5C000, 8);
DEFINE_RCG_MND(mclk3_clk_src, g0_g6_g2_g0d_g6d, ftbl_mclk3_clk_src, 0x5E000, 8);
DEFINE_RCG_MND(usb30_mock_utmi_clk_src, xo_g0_g6d_g0d_g4_g0d_g6d,
		ftbl_usb30_mock_utmi_clk_src, 0x3F020, 8);
DEFINE_RCG_MND(usb3_aux_clk_src, xo_g0_g4_g0d, ftbl_usb3_aux_clk_src, 0x3F05C, 8);
DEFINE_RCG_COMMON(sdcc1_apps_clk_src, xo_g0_g4_g0d, ftbl_sdcc1_apps_clk_src,
		clk_rcg2_floor_ops, 0x42004, 8, 0);
DEFINE_RCG_COMMON(sdcc2_apps_clk_src, xo_g0_g4_g0d, ftbl_sdcc2_apps_clk_src,
		clk_rcg2_floor_ops, 0x43004, 8, 0);
DEFINE_RCG_COMMON(pclk0_clk_src, xo_dsi0pll_dsi1pll, NULL,
		clk_pixel_ops, 0x4D000, 8, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED);
DEFINE_RCG_COMMON(pclk1_clk_src, xo_dsi1pll_dsi0pll, NULL,
		clk_pixel_ops, 0x4D0B8, 8, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED);
DEFINE_RCG_COMMON(byte0_clk_src, xo_dsi0pllbyte_dsi1pllbyte, NULL,
		clk_byte2_ops, 0x4D044, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED);
DEFINE_RCG_COMMON(byte1_clk_src, xo_dsi1pllbyte_dsi0pllbyte, NULL,
		clk_byte2_ops, 0x4D0B0, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED);

#define DEFINE_BRANCH_COMMON(_name, _parent_hw, _parent_name,			\
			     _cbcr, _enable_bit,				\
				_flags,	_halt_check, _halt_reg)			\
	static struct clk_branch _name = {					\
		.halt_reg = _halt_reg,						\
		.halt_check = _halt_check,					\
		.clkr = {							\
			.enable_reg = _cbcr,					\
			.enable_mask = BIT(_enable_bit),			\
			.hw.init = &(struct clk_init_data) {			\
				.num_parents = 1,				\
				.parent_data = &(const struct clk_parent_data) {	\
					.fw_name = _parent_name,		\
					.name = _parent_name,			\
					.hw = _parent_hw,			\
				},						\
				.name = #_name,					\
				.ops = &clk_branch2_ops,			\
				.flags = _flags,				\
			}							\
		}								\
	}

#define DEFINE_BRANCH(_name, _parent, _cbcr)					\
	DEFINE_BRANCH_COMMON(_name, &_parent.clkr.hw, NULL, _cbcr, 0,		\
			     CLK_SET_RATE_PARENT, BRANCH_HALT, _cbcr)

#define DEFINE_BRANCH_HALT(_name, _parent, _cbcr, _enable_bit,			\
			   _halt_check, _halt_reg)				\
	DEFINE_BRANCH_COMMON(_name, &_parent.clkr.hw, NULL, _cbcr, _enable_bit,	\
			     CLK_SET_RATE_PARENT, _halt_check, _halt_reg)

#define DEFINE_BRANCH_EXT(_name, _parent_name, _cbcr)				\
	DEFINE_BRANCH_COMMON(_name, NULL, _parent_name, _cbcr, 0,		\
			     0, BRANCH_HALT, _cbcr)

#define DEFINE_BRANCH_EXT_HALT(_name, _parent_name, _cbcr, _enable_bit,		\
			       _halt_check, _halt_reg)				\
	DEFINE_BRANCH_COMMON(_name, NULL, _parent_name, _cbcr, _enable_bit,	\
			     0, _halt_check, _halt_reg)

DEFINE_BRANCH(gcc_blsp1_uart1_apps_clk, blsp1_uart1_apps_clk_src, 0x0203C);
DEFINE_BRANCH(gcc_blsp1_uart2_apps_clk, blsp2_uart2_apps_clk_src, 0x0302C);
DEFINE_BRANCH(gcc_blsp2_uart1_apps_clk, blsp1_uart1_apps_clk_src, 0x0C03C);
DEFINE_BRANCH(gcc_blsp2_uart2_apps_clk, blsp2_uart2_apps_clk_src, 0x0D02C);
DEFINE_BRANCH(gcc_blsp1_qup1_i2c_apps_clk, blsp1_qup1_i2c_apps_clk_src, 0x02008);
DEFINE_BRANCH(gcc_blsp1_qup2_i2c_apps_clk, blsp1_qup2_i2c_apps_clk_src, 0x03010);
DEFINE_BRANCH(gcc_blsp1_qup3_i2c_apps_clk, blsp1_qup3_i2c_apps_clk_src, 0x04020);
DEFINE_BRANCH(gcc_blsp1_qup4_i2c_apps_clk, blsp1_qup4_i2c_apps_clk_src, 0x05020);
DEFINE_BRANCH(gcc_blsp2_qup1_i2c_apps_clk, blsp2_qup1_i2c_apps_clk_src, 0x0C008);
DEFINE_BRANCH(gcc_blsp2_qup2_i2c_apps_clk, blsp2_qup2_i2c_apps_clk_src, 0x0D010);
DEFINE_BRANCH(gcc_blsp2_qup3_i2c_apps_clk, blsp2_qup3_i2c_apps_clk_src, 0x0F020);
DEFINE_BRANCH(gcc_blsp2_qup4_i2c_apps_clk, blsp2_qup4_i2c_apps_clk_src, 0x18020);
DEFINE_BRANCH(gcc_blsp1_qup1_spi_apps_clk, blsp1_qup1_spi_apps_clk_src, 0x02004);
DEFINE_BRANCH(gcc_blsp1_qup2_spi_apps_clk, blsp1_qup2_spi_apps_clk_src, 0x0300C);
DEFINE_BRANCH(gcc_blsp1_qup3_spi_apps_clk, blsp1_qup3_spi_apps_clk_src, 0x0401C);
DEFINE_BRANCH(gcc_blsp1_qup4_spi_apps_clk, blsp1_qup4_spi_apps_clk_src, 0x0501C);
DEFINE_BRANCH(gcc_blsp2_qup1_spi_apps_clk, blsp2_qup1_spi_apps_clk_src, 0x0C004);
DEFINE_BRANCH(gcc_blsp2_qup2_spi_apps_clk, blsp2_qup2_spi_apps_clk_src, 0x0D00C);
DEFINE_BRANCH(gcc_blsp2_qup3_spi_apps_clk, blsp2_qup3_spi_apps_clk_src, 0x0F01C);
DEFINE_BRANCH(gcc_blsp2_qup4_spi_apps_clk, blsp2_qup4_spi_apps_clk_src, 0x1801C);
DEFINE_BRANCH(gcc_camss_cci_ahb_clk, camss_top_ahb_clk_src, 0x5101C);
DEFINE_BRANCH(gcc_camss_cci_clk, cci_clk_src, 0x51018);
DEFINE_BRANCH(gcc_camss_cpp_ahb_clk, camss_top_ahb_clk_src, 0x58040);
DEFINE_BRANCH(gcc_camss_cpp_clk, cpp_clk_src, 0x5803C);
DEFINE_BRANCH(gcc_camss_csi0_ahb_clk, camss_top_ahb_clk_src, 0x4E040);
DEFINE_BRANCH(gcc_camss_csi0_clk, csi0_clk_src, 0x4E03C);
DEFINE_BRANCH(gcc_camss_csi0_csiphy_3p_clk, csi0p_clk_src, 0x58090);
DEFINE_BRANCH(gcc_camss_csi0phy_clk, csi0_clk_src, 0x4E048);
DEFINE_BRANCH(gcc_camss_csi0pix_clk, csi0_clk_src, 0x4E058);
DEFINE_BRANCH(gcc_camss_csi0rdi_clk, csi0_clk_src, 0x4E050);
DEFINE_BRANCH(gcc_camss_csi1_ahb_clk, camss_top_ahb_clk_src, 0x4F040);
DEFINE_BRANCH(gcc_camss_csi1_clk, csi1_clk_src, 0x4F03C);
DEFINE_BRANCH(gcc_camss_csi1_csiphy_3p_clk, csi1p_clk_src, 0x580A0);
DEFINE_BRANCH(gcc_camss_csi1phy_clk, csi1_clk_src, 0x4F048);
DEFINE_BRANCH(gcc_camss_csi1pix_clk, csi1_clk_src, 0x4F058);
DEFINE_BRANCH(gcc_camss_csi1rdi_clk, csi1_clk_src, 0x4F050);
DEFINE_BRANCH(gcc_camss_csi2_ahb_clk, camss_top_ahb_clk_src, 0x3C040);
DEFINE_BRANCH(gcc_camss_csi2_clk, csi2_clk_src, 0x3C03C);
DEFINE_BRANCH(gcc_camss_csi2_csiphy_3p_clk, csi2p_clk_src, 0x580B0);
DEFINE_BRANCH(gcc_camss_csi2phy_clk, csi2_clk_src, 0x3C048);
DEFINE_BRANCH(gcc_camss_csi2pix_clk, csi2_clk_src, 0x3C058);
DEFINE_BRANCH(gcc_camss_csi2rdi_clk, csi2_clk_src, 0x3C050);
DEFINE_BRANCH(gcc_camss_csi_vfe0_clk, vfe0_clk_src, 0x58050);
DEFINE_BRANCH(gcc_camss_csi_vfe1_clk, vfe1_clk_src, 0x58074);
DEFINE_BRANCH(gcc_camss_gp0_clk, camss_gp0_clk_src, 0x54018);
DEFINE_BRANCH(gcc_camss_gp1_clk, camss_gp1_clk_src, 0x55018);
DEFINE_BRANCH(gcc_camss_ispif_ahb_clk, camss_top_ahb_clk_src, 0x50004);
DEFINE_BRANCH(gcc_camss_jpeg0_clk, jpeg0_clk_src, 0x57020);
DEFINE_BRANCH(gcc_camss_jpeg_ahb_clk, camss_top_ahb_clk_src, 0x57024);
DEFINE_BRANCH(gcc_camss_mclk0_clk, mclk0_clk_src, 0x52018);
DEFINE_BRANCH(gcc_camss_mclk1_clk, mclk1_clk_src, 0x53018);
DEFINE_BRANCH(gcc_camss_mclk2_clk, mclk2_clk_src, 0x5C018);
DEFINE_BRANCH(gcc_camss_mclk3_clk, mclk3_clk_src, 0x5E018);
DEFINE_BRANCH(gcc_camss_micro_ahb_clk, camss_top_ahb_clk_src, 0x5600C);
DEFINE_BRANCH(gcc_camss_csi0phytimer_clk, csi0phytimer_clk_src, 0x4E01C);
DEFINE_BRANCH(gcc_camss_csi1phytimer_clk, csi1phytimer_clk_src, 0x4F01C);
DEFINE_BRANCH(gcc_camss_csi2phytimer_clk, csi2phytimer_clk_src, 0x4F068);
DEFINE_BRANCH(gcc_camss_top_ahb_clk, camss_top_ahb_clk_src, 0x5A014);
DEFINE_BRANCH(gcc_camss_vfe0_clk, vfe0_clk_src, 0x58038);
DEFINE_BRANCH(gcc_camss_vfe_ahb_clk, camss_top_ahb_clk_src, 0x58044);
DEFINE_BRANCH(gcc_camss_vfe1_ahb_clk, camss_top_ahb_clk_src, 0x58060);
DEFINE_BRANCH(gcc_camss_vfe1_clk, vfe1_clk_src, 0x5805C);
DEFINE_BRANCH(gcc_gp1_clk, gp1_clk_src, 0x08000);
DEFINE_BRANCH(gcc_gp2_clk, gp2_clk_src, 0x09000);
DEFINE_BRANCH(gcc_gp3_clk, gp3_clk_src, 0x0A000);
DEFINE_BRANCH(gcc_mdss_esc0_clk, esc0_clk_src, 0x4D098);
DEFINE_BRANCH(gcc_mdss_esc1_clk, esc1_clk_src, 0x4D09C);
DEFINE_BRANCH(gcc_mdss_mdp_clk, mdp_clk_src, 0x4D088);
DEFINE_BRANCH(gcc_mdss_vsync_clk, vsync_clk_src, 0x4D090);
DEFINE_BRANCH(gcc_oxili_gfx3d_clk, gfx3d_clk_src, 0x59020);
DEFINE_BRANCH(gcc_pcnoc_usb3_axi_clk, usb30_master_clk_src, 0x3F038);
DEFINE_BRANCH(gcc_pdm2_clk, pdm2_clk_src, 0x4400C);
DEFINE_BRANCH(gcc_rbcpr_gfx_clk, rbcpr_gfx_clk_src, 0x3A004);
DEFINE_BRANCH(gcc_sdcc1_apps_clk, sdcc1_apps_clk_src, 0x42018);
DEFINE_BRANCH(gcc_sdcc1_ice_core_clk, sdcc1_ice_core_clk_src, 0x5D014);
DEFINE_BRANCH(gcc_sdcc2_apps_clk, sdcc2_apps_clk_src, 0x43018);
DEFINE_BRANCH(gcc_usb30_master_clk, usb30_master_clk_src, 0x3F000);
DEFINE_BRANCH(gcc_usb30_mock_utmi_clk, usb30_mock_utmi_clk_src, 0x3F008);
DEFINE_BRANCH(gcc_usb3_aux_clk, usb3_aux_clk_src, 0x3F044);
DEFINE_BRANCH(gcc_venus0_core0_vcodec0_clk, vcodec0_clk_src, 0x4C02C);
DEFINE_BRANCH(gcc_venus0_vcodec0_clk, vcodec0_clk_src, 0x4C01C);
DEFINE_BRANCH(gcc_apc0_droop_detector_gpll0_clk, apc0_droop_detector_clk_src, 0x78004);
DEFINE_BRANCH(gcc_apc1_droop_detector_gpll0_clk, apc1_droop_detector_clk_src, 0x79004);
DEFINE_BRANCH_HALT(gcc_apss_ahb_clk, apss_ahb_clk_src, 0x45004, 14, BRANCH_HALT_VOTED, 0x4601C);
DEFINE_BRANCH_HALT(gcc_crypto_clk, crypto_clk_src, 0x45004, 2, BRANCH_HALT_VOTED, 0x1601C);
DEFINE_BRANCH_EXT(gcc_bimc_gpu_clk, "bimc", 0x59030);
DEFINE_BRANCH_EXT(gcc_camss_cpp_axi_clk, "xo", 0x58064);
DEFINE_BRANCH_EXT(gcc_camss_jpeg_axi_clk, "xo", 0x57028);
DEFINE_BRANCH_EXT(gcc_camss_ahb_clk, "pcnoc", 0x56004);
DEFINE_BRANCH_EXT(gcc_camss_vfe_axi_clk, "xo", 0x58048);
DEFINE_BRANCH_EXT(gcc_camss_vfe1_axi_clk, "xo", 0x58068);
DEFINE_BRANCH_EXT(gcc_dcc_clk, "pcnoc", 0x77004);
DEFINE_BRANCH_EXT(gcc_mdss_ahb_clk, "pcnoc", 0x4D07C);
DEFINE_BRANCH_EXT(gcc_mdss_axi_clk, "sysmnoc", 0x4D080);
DEFINE_BRANCH_EXT(gcc_mss_cfg_ahb_clk, "pcnoc", 0x49000);
DEFINE_BRANCH_EXT(gcc_mss_q6_bimc_axi_clk, "bimc", 0x49004);
DEFINE_BRANCH_EXT(gcc_bimc_gfx_clk, "bimc", 0x59034);
DEFINE_BRANCH_EXT(gcc_oxili_ahb_clk, "xo", 0x59028);
DEFINE_BRANCH_EXT(gcc_oxili_timer_clk, "xo", 0x59040);
DEFINE_BRANCH_EXT(gcc_pdm_ahb_clk, "pcnoc", 0x44004);
DEFINE_BRANCH_EXT(gcc_sdcc1_ahb_clk, "pcnoc", 0x4201C);
DEFINE_BRANCH_EXT(gcc_sdcc2_ahb_clk, "pcnoc", 0x4301C);
DEFINE_BRANCH_EXT(gcc_usb30_sleep_clk, "xo", 0x3F004);
DEFINE_BRANCH_EXT(gcc_venus0_ahb_clk, "xo", 0x4C020);
DEFINE_BRANCH_EXT(gcc_venus0_axi_clk, "xo", 0x4C024);
DEFINE_BRANCH_EXT_HALT(gcc_usb3_pipe_clk, "xo", 0x3F040, 0, BRANCH_HALT_DELAY, 0);
DEFINE_BRANCH_EXT_HALT(gcc_usb_phy_cfg_ahb_clk, "pcnoc", 0x3F080, 0, BRANCH_VOTED, 0x3F080);
DEFINE_BRANCH_EXT_HALT(gcc_qusb_ref_clk, "bb_clk1", 0x41030, 0, BRANCH_HALT_SKIP, 0);
DEFINE_BRANCH_EXT_HALT(gcc_usb_ss_ref_clk, "bb_clk1", 0x3F07C, 0, BRANCH_HALT_SKIP, 0);
DEFINE_BRANCH_EXT_HALT(gcc_apss_axi_clk, "bimc", 0x45004, 13, BRANCH_HALT_VOTED, 0x46020);
DEFINE_BRANCH_EXT_HALT(gcc_blsp1_ahb_clk, "pcnoc", 0x45004, 10, BRANCH_HALT_VOTED, 0x01008);
DEFINE_BRANCH_EXT_HALT(gcc_blsp2_ahb_clk, "pcnoc", 0x45004, 20, BRANCH_HALT_VOTED, 0x0B008);
DEFINE_BRANCH_EXT_HALT(gcc_boot_rom_ahb_clk, "pcnoc", 0x45004, 7, BRANCH_HALT_VOTED, 0x1300C);
DEFINE_BRANCH_EXT_HALT(gcc_crypto_ahb_clk, "pcnoc", 0x45004, 0, BRANCH_HALT_VOTED, 0x16024);
DEFINE_BRANCH_EXT_HALT(gcc_crypto_axi_clk, "pcnoc", 0x45004, 1, BRANCH_HALT_VOTED, 0x16020);
DEFINE_BRANCH_EXT_HALT(gcc_qdss_dap_clk, "xo", 0x45004, 11, BRANCH_HALT_VOTED, 0x29084);
DEFINE_BRANCH_EXT_HALT(gcc_prng_ahb_clk, "pcnoc", 0x45004, 8, BRANCH_HALT_VOTED, 0x13004);
DEFINE_BRANCH_EXT_HALT(gcc_apss_tcu_async_clk, "bimc", 0x4500C, 1, BRANCH_HALT_VOTED, 0x12018);
DEFINE_BRANCH_EXT_HALT(gcc_cpp_tbu_clk, "sysmnoc", 0x4500C, 14, BRANCH_HALT_VOTED, 0x12040);
DEFINE_BRANCH_EXT_HALT(gcc_jpeg_tbu_clk, "sysmnoc", 0x4500C, 10, BRANCH_HALT_VOTED, 0x12034);
DEFINE_BRANCH_EXT_HALT(gcc_mdp_tbu_clk, "sysmnoc", 0x4500C, 4, BRANCH_HALT_VOTED, 0x1201C);
DEFINE_BRANCH_EXT_HALT(gcc_smmu_cfg_clk, "pcnoc", 0x4500C, 12, BRANCH_HALT_VOTED, 0x12038);
DEFINE_BRANCH_EXT_HALT(gcc_venus_tbu_clk, "sysmnoc", 0x4500C, 5, BRANCH_HALT_VOTED, 0x12014);
DEFINE_BRANCH_EXT_HALT(gcc_vfe1_tbu_clk, "sysmnoc", 0x4500C, 17, BRANCH_HALT_VOTED, 0x12090);
DEFINE_BRANCH_EXT_HALT(gcc_vfe_tbu_clk, "sysmnoc", 0x4500C, 9, BRANCH_HALT_VOTED, 0x1203C);
DEFINE_BRANCH_COMMON(gcc_oxili_aon_clk, &gfx3d_clk_src.clkr.hw, NULL,
		     0x59044, 0, 0, BRANCH_HALT, 0x59044);
DEFINE_BRANCH_COMMON(gcc_mdss_byte0_clk, &byte0_clk_src.clkr.hw, NULL,
		     0x4D094, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED, BRANCH_HALT, 0x4D094);
DEFINE_BRANCH_COMMON(gcc_mdss_byte1_clk, &byte1_clk_src.clkr.hw, NULL,
		     0x4D0A0, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED, BRANCH_HALT, 0x4D0A0);
DEFINE_BRANCH_COMMON(gcc_mdss_pclk0_clk, &pclk0_clk_src.clkr.hw, NULL,
		     0x4D084, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED, BRANCH_HALT, 0x4D084);
DEFINE_BRANCH_COMMON(gcc_mdss_pclk1_clk, &pclk1_clk_src.clkr.hw, NULL,
		     0x4D0A4, 0, CLK_SET_RATE_PARENT|CLK_IGNORE_UNUSED, BRANCH_HALT, 0x4D0A4);


static struct gdsc usb30_gdsc = {
	.gdscr = 0x3f078,
	.pd = {
		.name = "usb30_gdsc",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc venus_gdsc = {
	.gdscr = 0x4c018,
	.pd = {
		.name = "venus",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc venus_core0_gdsc = {
	.gdscr = 0x4c028,
	.pd = {
		.name = "venus_core0",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc mdss_gdsc = {
	.gdscr = 0x4d078,
	.pd = {
		.name = "mdss",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc jpeg_gdsc = {
	.gdscr = 0x5701c,
	.pd = {
		.name = "jpeg",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc vfe_gdsc = {
	.gdscr = 0x58034,
	.pd = {
		.name = "vfe",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct gdsc oxili_gx_gdsc = {
	.gdscr = 0x5901c,
	.clamp_io_ctrl = 0x5b00c,
	.pd = {
		.name = "oxili_gx",
	},
	.pwrsts = PWRSTS_OFF_ON,
	.flags = CLAMP_IO,
};

static struct gdsc oxili_cx_gdsc = {
	.gdscr = 0x5904c,
	.pd = {
		.name = "oxili_cx",
	},
	.pwrsts = PWRSTS_OFF_ON,
};

static struct clk_hw *gcc_msm8953_hws[] = {
	&gpll0_early_div.hw,
	&gpll3_early_div.hw,
	&gpll6_early_div.hw,
};

static struct clk_regmap *gcc_msm8953_clocks[] = {
	[GPLL0] = &gpll0.clkr,
	[GPLL0_EARLY] = &gpll0_early.clkr,
	[GPLL2] = &gpll2.clkr,
	[GPLL2_EARLY] = &gpll2_early.clkr,
	[GPLL3] = &gpll3.clkr,
	[GPLL3_EARLY] = &gpll3_early.clkr,
	[GPLL4] = &gpll4.clkr,
	[GPLL4_EARLY] = &gpll4_early.clkr,
	[GPLL6] = &gpll6.clkr,
	[GPLL6_EARLY] = &gpll6_early.clkr,
	[GCC_APSS_AHB_CLK] = &gcc_apss_ahb_clk.clkr,
	[GCC_APSS_AXI_CLK] = &gcc_apss_axi_clk.clkr,
	[GCC_BLSP1_AHB_CLK] = &gcc_blsp1_ahb_clk.clkr,
	[GCC_BLSP2_AHB_CLK] = &gcc_blsp2_ahb_clk.clkr,
	[GCC_BOOT_ROM_AHB_CLK] = &gcc_boot_rom_ahb_clk.clkr,
	[GCC_CRYPTO_AHB_CLK] = &gcc_crypto_ahb_clk.clkr,
	[GCC_CRYPTO_AXI_CLK] = &gcc_crypto_axi_clk.clkr,
	[GCC_CRYPTO_CLK] = &gcc_crypto_clk.clkr,
	[GCC_PRNG_AHB_CLK] = &gcc_prng_ahb_clk.clkr,
	[GCC_QDSS_DAP_CLK] = &gcc_qdss_dap_clk.clkr,
	[GCC_APSS_TCU_ASYNC_CLK] = &gcc_apss_tcu_async_clk.clkr,
	[GCC_CPP_TBU_CLK] = &gcc_cpp_tbu_clk.clkr,
	[GCC_JPEG_TBU_CLK] = &gcc_jpeg_tbu_clk.clkr,
	[GCC_MDP_TBU_CLK] = &gcc_mdp_tbu_clk.clkr,
	[GCC_SMMU_CFG_CLK] = &gcc_smmu_cfg_clk.clkr,
	[GCC_VENUS_TBU_CLK] = &gcc_venus_tbu_clk.clkr,
	[GCC_VFE1_TBU_CLK] = &gcc_vfe1_tbu_clk.clkr,
	[GCC_VFE_TBU_CLK] = &gcc_vfe_tbu_clk.clkr,
	[CAMSS_TOP_AHB_CLK_SRC] = &camss_top_ahb_clk_src.clkr,
	[CSI0_CLK_SRC] = &csi0_clk_src.clkr,
	[APSS_AHB_CLK_SRC] = &apss_ahb_clk_src.clkr,
	[CSI1_CLK_SRC] = &csi1_clk_src.clkr,
	[CSI2_CLK_SRC] = &csi2_clk_src.clkr,
	[VFE0_CLK_SRC] = &vfe0_clk_src.clkr,
	[VCODEC0_CLK_SRC] = &vcodec0_clk_src.clkr,
	[CPP_CLK_SRC] = &cpp_clk_src.clkr,
	[JPEG0_CLK_SRC] = &jpeg0_clk_src.clkr,
	[USB30_MASTER_CLK_SRC] = &usb30_master_clk_src.clkr,
	[VFE1_CLK_SRC] = &vfe1_clk_src.clkr,
	[APC0_DROOP_DETECTOR_CLK_SRC] = &apc0_droop_detector_clk_src.clkr,
	[APC1_DROOP_DETECTOR_CLK_SRC] = &apc1_droop_detector_clk_src.clkr,
	[BLSP1_QUP1_I2C_APPS_CLK_SRC] = &blsp1_qup1_i2c_apps_clk_src.clkr,
	[BLSP1_QUP1_SPI_APPS_CLK_SRC] = &blsp1_qup1_spi_apps_clk_src.clkr,
	[BLSP1_QUP2_I2C_APPS_CLK_SRC] = &blsp1_qup2_i2c_apps_clk_src.clkr,
	[BLSP1_QUP2_SPI_APPS_CLK_SRC] = &blsp1_qup2_spi_apps_clk_src.clkr,
	[BLSP1_QUP3_I2C_APPS_CLK_SRC] = &blsp1_qup3_i2c_apps_clk_src.clkr,
	[BLSP1_QUP3_SPI_APPS_CLK_SRC] = &blsp1_qup3_spi_apps_clk_src.clkr,
	[BLSP1_QUP4_I2C_APPS_CLK_SRC] = &blsp1_qup4_i2c_apps_clk_src.clkr,
	[BLSP1_QUP4_SPI_APPS_CLK_SRC] = &blsp1_qup4_spi_apps_clk_src.clkr,
	[BLSP1_UART1_APPS_CLK_SRC] = &blsp1_uart1_apps_clk_src.clkr,
	[BLSP1_UART2_APPS_CLK_SRC] = &blsp1_uart2_apps_clk_src.clkr,
	[BLSP2_QUP1_I2C_APPS_CLK_SRC] = &blsp2_qup1_i2c_apps_clk_src.clkr,
	[BLSP2_QUP1_SPI_APPS_CLK_SRC] = &blsp2_qup1_spi_apps_clk_src.clkr,
	[BLSP2_QUP2_I2C_APPS_CLK_SRC] = &blsp2_qup2_i2c_apps_clk_src.clkr,
	[BLSP2_QUP2_SPI_APPS_CLK_SRC] = &blsp2_qup2_spi_apps_clk_src.clkr,
	[BLSP2_QUP3_I2C_APPS_CLK_SRC] = &blsp2_qup3_i2c_apps_clk_src.clkr,
	[BLSP2_QUP3_SPI_APPS_CLK_SRC] = &blsp2_qup3_spi_apps_clk_src.clkr,
	[BLSP2_QUP4_I2C_APPS_CLK_SRC] = &blsp2_qup4_i2c_apps_clk_src.clkr,
	[BLSP2_QUP4_SPI_APPS_CLK_SRC] = &blsp2_qup4_spi_apps_clk_src.clkr,
	[BLSP2_UART1_APPS_CLK_SRC] = &blsp2_uart1_apps_clk_src.clkr,
	[BLSP2_UART2_APPS_CLK_SRC] = &blsp2_uart2_apps_clk_src.clkr,
	[CCI_CLK_SRC] = &cci_clk_src.clkr,
	[CSI0P_CLK_SRC] = &csi0p_clk_src.clkr,
	[CSI1P_CLK_SRC] = &csi1p_clk_src.clkr,
	[CSI2P_CLK_SRC] = &csi2p_clk_src.clkr,
	[CAMSS_GP0_CLK_SRC] = &camss_gp0_clk_src.clkr,
	[CAMSS_GP1_CLK_SRC] = &camss_gp1_clk_src.clkr,
	[MCLK0_CLK_SRC] = &mclk0_clk_src.clkr,
	[MCLK1_CLK_SRC] = &mclk1_clk_src.clkr,
	[MCLK2_CLK_SRC] = &mclk2_clk_src.clkr,
	[MCLK3_CLK_SRC] = &mclk3_clk_src.clkr,
	[CSI0PHYTIMER_CLK_SRC] = &csi0phytimer_clk_src.clkr,
	[CSI1PHYTIMER_CLK_SRC] = &csi1phytimer_clk_src.clkr,
	[CSI2PHYTIMER_CLK_SRC] = &csi2phytimer_clk_src.clkr,
	[CRYPTO_CLK_SRC] = &crypto_clk_src.clkr,
	[GP1_CLK_SRC] = &gp1_clk_src.clkr,
	[GP2_CLK_SRC] = &gp2_clk_src.clkr,
	[GP3_CLK_SRC] = &gp3_clk_src.clkr,
	[PDM2_CLK_SRC] = &pdm2_clk_src.clkr,
	[RBCPR_GFX_CLK_SRC] = &rbcpr_gfx_clk_src.clkr,
	[SDCC1_APPS_CLK_SRC] = &sdcc1_apps_clk_src.clkr,
	[SDCC1_ICE_CORE_CLK_SRC] = &sdcc1_ice_core_clk_src.clkr,
	[SDCC2_APPS_CLK_SRC] = &sdcc2_apps_clk_src.clkr,
	[USB30_MOCK_UTMI_CLK_SRC] = &usb30_mock_utmi_clk_src.clkr,
	[USB3_AUX_CLK_SRC] = &usb3_aux_clk_src.clkr,
	[GCC_APC0_DROOP_DETECTOR_GPLL0_CLK] = &gcc_apc0_droop_detector_gpll0_clk.clkr,
	[GCC_APC1_DROOP_DETECTOR_GPLL0_CLK] = &gcc_apc1_droop_detector_gpll0_clk.clkr,
	[GCC_BLSP1_QUP1_I2C_APPS_CLK] = &gcc_blsp1_qup1_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP1_SPI_APPS_CLK] = &gcc_blsp1_qup1_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP2_I2C_APPS_CLK] = &gcc_blsp1_qup2_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP2_SPI_APPS_CLK] = &gcc_blsp1_qup2_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP3_I2C_APPS_CLK] = &gcc_blsp1_qup3_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP3_SPI_APPS_CLK] = &gcc_blsp1_qup3_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP4_I2C_APPS_CLK] = &gcc_blsp1_qup4_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP4_SPI_APPS_CLK] = &gcc_blsp1_qup4_spi_apps_clk.clkr,
	[GCC_BLSP1_UART1_APPS_CLK] = &gcc_blsp1_uart1_apps_clk.clkr,
	[GCC_BLSP1_UART2_APPS_CLK] = &gcc_blsp1_uart2_apps_clk.clkr,
	[GCC_BLSP2_QUP1_I2C_APPS_CLK] = &gcc_blsp2_qup1_i2c_apps_clk.clkr,
	[GCC_BLSP2_QUP1_SPI_APPS_CLK] = &gcc_blsp2_qup1_spi_apps_clk.clkr,
	[GCC_BLSP2_QUP2_I2C_APPS_CLK] = &gcc_blsp2_qup2_i2c_apps_clk.clkr,
	[GCC_BLSP2_QUP2_SPI_APPS_CLK] = &gcc_blsp2_qup2_spi_apps_clk.clkr,
	[GCC_BLSP2_QUP3_I2C_APPS_CLK] = &gcc_blsp2_qup3_i2c_apps_clk.clkr,
	[GCC_BLSP2_QUP3_SPI_APPS_CLK] = &gcc_blsp2_qup3_spi_apps_clk.clkr,
	[GCC_BLSP2_QUP4_I2C_APPS_CLK] = &gcc_blsp2_qup4_i2c_apps_clk.clkr,
	[GCC_BLSP2_QUP4_SPI_APPS_CLK] = &gcc_blsp2_qup4_spi_apps_clk.clkr,
	[GCC_BLSP2_UART1_APPS_CLK] = &gcc_blsp2_uart1_apps_clk.clkr,
	[GCC_BLSP2_UART2_APPS_CLK] = &gcc_blsp2_uart2_apps_clk.clkr,
	[GCC_CAMSS_CCI_AHB_CLK] = &gcc_camss_cci_ahb_clk.clkr,
	[GCC_CAMSS_CCI_CLK] = &gcc_camss_cci_clk.clkr,
	[GCC_CAMSS_CPP_AHB_CLK] = &gcc_camss_cpp_ahb_clk.clkr,
	[GCC_CAMSS_CPP_AXI_CLK] = &gcc_camss_cpp_axi_clk.clkr,
	[GCC_CAMSS_CPP_CLK] = &gcc_camss_cpp_clk.clkr,
	[GCC_CAMSS_CSI0_AHB_CLK] = &gcc_camss_csi0_ahb_clk.clkr,
	[GCC_CAMSS_CSI0_CLK] = &gcc_camss_csi0_clk.clkr,
	[GCC_CAMSS_CSI0_CSIPHY_3P_CLK] = &gcc_camss_csi0_csiphy_3p_clk.clkr,
	[GCC_CAMSS_CSI0PHY_CLK] = &gcc_camss_csi0phy_clk.clkr,
	[GCC_CAMSS_CSI0PIX_CLK] = &gcc_camss_csi0pix_clk.clkr,
	[GCC_CAMSS_CSI0RDI_CLK] = &gcc_camss_csi0rdi_clk.clkr,
	[GCC_CAMSS_CSI1_AHB_CLK] = &gcc_camss_csi1_ahb_clk.clkr,
	[GCC_CAMSS_CSI1_CLK] = &gcc_camss_csi1_clk.clkr,
	[GCC_CAMSS_CSI1_CSIPHY_3P_CLK] = &gcc_camss_csi1_csiphy_3p_clk.clkr,
	[GCC_CAMSS_CSI1PHY_CLK] = &gcc_camss_csi1phy_clk.clkr,
	[GCC_CAMSS_CSI1PIX_CLK] = &gcc_camss_csi1pix_clk.clkr,
	[GCC_CAMSS_CSI1RDI_CLK] = &gcc_camss_csi1rdi_clk.clkr,
	[GCC_CAMSS_CSI2_AHB_CLK] = &gcc_camss_csi2_ahb_clk.clkr,
	[GCC_CAMSS_CSI2_CLK] = &gcc_camss_csi2_clk.clkr,
	[GCC_CAMSS_CSI2_CSIPHY_3P_CLK] = &gcc_camss_csi2_csiphy_3p_clk.clkr,
	[GCC_CAMSS_CSI2PHY_CLK] = &gcc_camss_csi2phy_clk.clkr,
	[GCC_CAMSS_CSI2PIX_CLK] = &gcc_camss_csi2pix_clk.clkr,
	[GCC_CAMSS_CSI2RDI_CLK] = &gcc_camss_csi2rdi_clk.clkr,
	[GCC_CAMSS_CSI_VFE0_CLK] = &gcc_camss_csi_vfe0_clk.clkr,
	[GCC_CAMSS_CSI_VFE1_CLK] = &gcc_camss_csi_vfe1_clk.clkr,
	[GCC_CAMSS_GP0_CLK] = &gcc_camss_gp0_clk.clkr,
	[GCC_CAMSS_GP1_CLK] = &gcc_camss_gp1_clk.clkr,
	[GCC_CAMSS_ISPIF_AHB_CLK] = &gcc_camss_ispif_ahb_clk.clkr,
	[GCC_CAMSS_JPEG0_CLK] = &gcc_camss_jpeg0_clk.clkr,
	[GCC_CAMSS_JPEG_AHB_CLK] = &gcc_camss_jpeg_ahb_clk.clkr,
	[GCC_CAMSS_JPEG_AXI_CLK] = &gcc_camss_jpeg_axi_clk.clkr,
	[GCC_CAMSS_MCLK0_CLK] = &gcc_camss_mclk0_clk.clkr,
	[GCC_CAMSS_MCLK1_CLK] = &gcc_camss_mclk1_clk.clkr,
	[GCC_CAMSS_MCLK2_CLK] = &gcc_camss_mclk2_clk.clkr,
	[GCC_CAMSS_MCLK3_CLK] = &gcc_camss_mclk3_clk.clkr,
	[GCC_CAMSS_MICRO_AHB_CLK] = &gcc_camss_micro_ahb_clk.clkr,
	[GCC_CAMSS_CSI0PHYTIMER_CLK] = &gcc_camss_csi0phytimer_clk.clkr,
	[GCC_CAMSS_CSI1PHYTIMER_CLK] = &gcc_camss_csi1phytimer_clk.clkr,
	[GCC_CAMSS_CSI2PHYTIMER_CLK] = &gcc_camss_csi2phytimer_clk.clkr,
	[GCC_CAMSS_AHB_CLK] = &gcc_camss_ahb_clk.clkr,
	[GCC_CAMSS_TOP_AHB_CLK] = &gcc_camss_top_ahb_clk.clkr,
	[GCC_CAMSS_VFE0_CLK] = &gcc_camss_vfe0_clk.clkr,
	[GCC_CAMSS_VFE_AHB_CLK] = &gcc_camss_vfe_ahb_clk.clkr,
	[GCC_CAMSS_VFE_AXI_CLK] = &gcc_camss_vfe_axi_clk.clkr,
	[GCC_CAMSS_VFE1_AHB_CLK] = &gcc_camss_vfe1_ahb_clk.clkr,
	[GCC_CAMSS_VFE1_AXI_CLK] = &gcc_camss_vfe1_axi_clk.clkr,
	[GCC_CAMSS_VFE1_CLK] = &gcc_camss_vfe1_clk.clkr,
	[GCC_DCC_CLK] = &gcc_dcc_clk.clkr,
	[GCC_GP1_CLK] = &gcc_gp1_clk.clkr,
	[GCC_GP2_CLK] = &gcc_gp2_clk.clkr,
	[GCC_GP3_CLK] = &gcc_gp3_clk.clkr,
	[GCC_MSS_CFG_AHB_CLK] = &gcc_mss_cfg_ahb_clk.clkr,
	[GCC_MSS_Q6_BIMC_AXI_CLK] = &gcc_mss_q6_bimc_axi_clk.clkr,
	[GCC_PCNOC_USB3_AXI_CLK] = &gcc_pcnoc_usb3_axi_clk.clkr,
	[GCC_PDM2_CLK] = &gcc_pdm2_clk.clkr,
	[GCC_PDM_AHB_CLK] = &gcc_pdm_ahb_clk.clkr,
	[GCC_RBCPR_GFX_CLK] = &gcc_rbcpr_gfx_clk.clkr,
	[GCC_SDCC1_AHB_CLK] = &gcc_sdcc1_ahb_clk.clkr,
	[GCC_SDCC1_APPS_CLK] = &gcc_sdcc1_apps_clk.clkr,
	[GCC_SDCC1_ICE_CORE_CLK] = &gcc_sdcc1_ice_core_clk.clkr,
	[GCC_SDCC2_AHB_CLK] = &gcc_sdcc2_ahb_clk.clkr,
	[GCC_SDCC2_APPS_CLK] = &gcc_sdcc2_apps_clk.clkr,
	[GCC_USB30_MASTER_CLK] = &gcc_usb30_master_clk.clkr,
	[GCC_USB30_MOCK_UTMI_CLK] = &gcc_usb30_mock_utmi_clk.clkr,
	[GCC_USB30_SLEEP_CLK] = &gcc_usb30_sleep_clk.clkr,
	[GCC_USB3_AUX_CLK] = &gcc_usb3_aux_clk.clkr,
	[GCC_USB_PHY_CFG_AHB_CLK] = &gcc_usb_phy_cfg_ahb_clk.clkr,
	[GCC_VENUS0_AHB_CLK] = &gcc_venus0_ahb_clk.clkr,
	[GCC_VENUS0_AXI_CLK] = &gcc_venus0_axi_clk.clkr,
	[GCC_VENUS0_CORE0_VCODEC0_CLK] = &gcc_venus0_core0_vcodec0_clk.clkr,
	[GCC_VENUS0_VCODEC0_CLK] = &gcc_venus0_vcodec0_clk.clkr,
	[GCC_QUSB_REF_CLK] = &gcc_qusb_ref_clk.clkr,
	[GCC_USB_SS_REF_CLK] = &gcc_usb_ss_ref_clk.clkr,
	[GCC_USB3_PIPE_CLK] = &gcc_usb3_pipe_clk.clkr,
	[MDP_CLK_SRC] = &mdp_clk_src.clkr,
	[PCLK0_CLK_SRC] = &pclk0_clk_src.clkr,
	[BYTE0_CLK_SRC] = &byte0_clk_src.clkr,
	[ESC0_CLK_SRC] = &esc0_clk_src.clkr,
	[PCLK1_CLK_SRC] = &pclk1_clk_src.clkr,
	[BYTE1_CLK_SRC] = &byte1_clk_src.clkr,
	[ESC1_CLK_SRC] = &esc1_clk_src.clkr,
	[VSYNC_CLK_SRC] = &vsync_clk_src.clkr,
	[GCC_MDSS_AHB_CLK] = &gcc_mdss_ahb_clk.clkr,
	[GCC_MDSS_AXI_CLK] = &gcc_mdss_axi_clk.clkr,
	[GCC_MDSS_PCLK0_CLK] = &gcc_mdss_pclk0_clk.clkr,
	[GCC_MDSS_BYTE0_CLK] = &gcc_mdss_byte0_clk.clkr,
	[GCC_MDSS_ESC0_CLK] = &gcc_mdss_esc0_clk.clkr,
	[GCC_MDSS_PCLK1_CLK] = &gcc_mdss_pclk1_clk.clkr,
	[GCC_MDSS_BYTE1_CLK] = &gcc_mdss_byte1_clk.clkr,
	[GCC_MDSS_ESC1_CLK] = &gcc_mdss_esc1_clk.clkr,
	[GCC_MDSS_MDP_CLK] = &gcc_mdss_mdp_clk.clkr,
	[GCC_MDSS_VSYNC_CLK] = &gcc_mdss_vsync_clk.clkr,
	[GCC_OXILI_TIMER_CLK] = &gcc_oxili_timer_clk.clkr,
	[GCC_OXILI_GFX3D_CLK] = &gcc_oxili_gfx3d_clk.clkr,
	[GCC_OXILI_AON_CLK] = &gcc_oxili_aon_clk.clkr,
	[GCC_OXILI_AHB_CLK] = &gcc_oxili_ahb_clk.clkr,
	[GCC_BIMC_GFX_CLK] = &gcc_bimc_gfx_clk.clkr,
	[GCC_BIMC_GPU_CLK] = &gcc_bimc_gpu_clk.clkr,
	[GFX3D_CLK_SRC] = &gfx3d_clk_src.clkr,
};

static const struct qcom_reset_map gcc_msm8953_resets[] = {
	[GCC_CAMSS_MICRO_BCR]	= { 0x56008 },
	[GCC_MSS_BCR]		= { 0x71000 },
	[GCC_QUSB2_PHY_BCR]	= { 0x4103C },
	[GCC_USB3PHY_PHY_BCR]	= { 0x3F03C },
	[GCC_USB3_PHY_BCR]	= { 0x3F034 },
	[GCC_USB_30_BCR]	= { 0x3F070 },
};

static const struct regmap_config gcc_msm8953_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.max_register	= 0x80000,
	.fast_io	= true,
};

static struct gdsc *gcc_msm8953_gdscs[] = {
	[JPEG_GDSC] = &jpeg_gdsc,
	[MDSS_GDSC] = &mdss_gdsc,
	[OXILI_CX_GDSC] = &oxili_cx_gdsc,
	[OXILI_GX_GDSC] = &oxili_gx_gdsc,
	[USB30_GDSC] = &usb30_gdsc,
	[VENUS_CORE0_GDSC] = &venus_core0_gdsc,
	[VENUS_GDSC] = &venus_gdsc,
	[VFE_GDSC] = &vfe_gdsc,
};

static const struct qcom_cc_desc gcc_msm8953_desc = {
	.config = &gcc_msm8953_regmap_config,
	.clks = gcc_msm8953_clocks,
	.num_clks = ARRAY_SIZE(gcc_msm8953_clocks),
	.resets = gcc_msm8953_resets,
	.num_resets = ARRAY_SIZE(gcc_msm8953_resets),
	.gdscs = gcc_msm8953_gdscs,
	.num_gdscs = ARRAY_SIZE(gcc_msm8953_gdscs),
	.clk_hws = gcc_msm8953_hws,
	.num_clk_hws = ARRAY_SIZE(gcc_msm8953_hws),
};

static int gcc_msm8953_probe(struct platform_device *pdev)
{
	int ret = 0;

	ret = qcom_cc_probe(pdev, &gcc_msm8953_desc);

	if (ret < 0)
		return ret;

	clk_set_rate(gpll3_early.clkr.hw.clk, 68*19200000);

	return 0;
}

static const struct of_device_id gcc_msm8953_match_table[] = {
	{ .compatible = "qcom,gcc-msm8953" },
	{},
};

static struct platform_driver gcc_msm8953_driver = {
	.probe = gcc_msm8953_probe,
	.driver = {
		.name = "gcc-msm8953",
		.of_match_table = gcc_msm8953_match_table,
		.owner = THIS_MODULE,
	},
};

static int __init gcc_msm8953_init(void)
{
	return platform_driver_register(&gcc_msm8953_driver);
}
core_initcall(gcc_msm8953_init);

static void __exit gcc_msm8953_exit(void)
{
	platform_driver_unregister(&gcc_msm8953_driver);
}
module_exit(gcc_msm8953_exit);

MODULE_DESCRIPTION("Qualcomm GCC MSM8953 Driver");
MODULE_LICENSE("GPL v2");
