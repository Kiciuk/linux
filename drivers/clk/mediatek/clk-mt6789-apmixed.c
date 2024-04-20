// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include "clk-mtk.h"
#include "clk-gate.h"
#include "clk-mux.h"
#include "clk-pll.h"
#include <dt-bindings/clock/mediatek,mt6789-clk.h>


#define MT6789_PLL_FMAX		(3800UL * MHZ)
#define MT6789_PLL_FMIN		(1500UL * MHZ)
#define MT6789_INTEGER_BITS	8

#define PLL(_id, _name, _reg, _en_reg, _en_mask, _pll_en_bit,		\
			_pwr_reg, _flags, _rst_bar_mask,		\
			_pd_reg, _pd_shift, _tuner_reg,			\
			_tuner_en_reg, _tuner_en_bit,			\
			_pcw_reg, _pcw_shift, _pcwbits) {		\
		.id = _id,						\
		.name = _name,						\
		.reg = _reg,						\
		.en_reg = _en_reg,					\
		.en_mask = _en_mask,					\
		.pll_en_bit = _pll_en_bit,				\
		.pwr_reg = _pwr_reg,					\
		.flags = _flags,					\
		.rst_bar_mask = _rst_bar_mask,				\
		.fmax = MT6789_PLL_FMAX,				\
		.fmin = MT6789_PLL_FMIN,				\
		.pd_reg = _pd_reg,					\
		.pd_shift = _pd_shift,					\
		.tuner_reg = _tuner_reg,				\
		.tuner_en_reg = _tuner_en_reg,				\
		.tuner_en_bit = _tuner_en_bit,				\
		.pcw_reg = _pcw_reg,					\
		.pcw_shift = _pcw_shift,				\
		.pcwbits = _pcwbits,					\
		.pcwibits = MT6789_INTEGER_BITS,			\
	}

static const struct mtk_pll_data apmixed_plls[] = {
	PLL(CLK_APMIXED_ARMPLL_LL, "armpll_ll", 0x208 , 0x208, 0, 0 ,
	    0x214 , PLL_AO, BIT(0) , 0x20C, 24 ,
	    0, 0, 0 , 0x20C, 0, 22 ),
	PLL(CLK_APMIXED_ARMPLL_BL0, "armpll_bl0", 0x218 , 0x218, 0, 0 ,
	    0x224 , PLL_AO, BIT(0) , 0x21C, 24 ,
	    0, 0, 0 , 0x21C, 0, 22 ),
	PLL(CLK_APMIXED_CCIPLL, "ccipll", 0x258 , 0x258, 0, 0 ,
	    0x264 , PLL_AO, BIT(0) , 0x25C, 24 ,
	    0, 0, 0 , 0x25C, 0, 22 ),
	PLL(CLK_APMIXED_MPLL, "mpll", 0x390 , 0x390, 0, 0 ,
	    0x39C , PLL_AO, BIT(0) , 0x394, 24 ,
	    0, 0, 0 , 0x394, 0, 22 ),
	PLL(CLK_APMIXED_MAINPLL, "mainpll", 0x340 , 0x340, 0xff000000, 0 ,
	    0x34C , HAVE_RST_BAR | PLL_AO, BIT(23) , 0x344, 24 ,
	    0, 0, 0 , 0x344, 0, 22 ),
	PLL(CLK_APMIXED_UNIVPLL, "univpll", 0x308 , 0x308, 0xff000000, 0 ,
	    0x314 , HAVE_RST_BAR, BIT(23) , 0x30C, 24 ,
	    0, 0, 0 , 0x30C, 0, 22 ),
	PLL(CLK_APMIXED_MSDCPLL, "msdcpll", 0x350 , 0x350, 0, 0 ,
	    0x35C , 0, BIT(0) , 0x354, 24 ,
	    0, 0, 0 , 0x354, 0, 22 ),
	PLL(CLK_APMIXED_MMPLL, "mmpll", 0x360 , 0x360, 0xff000000, 0 ,
	    0x36C , HAVE_RST_BAR, BIT(23) , 0x364, 24 ,
	    0, 0, 0 , 0x364, 0, 22 ),
	PLL(CLK_APMIXED_NPUPLL, "npupll", 0x3B4 , 0x3B4, 0xff000000, 0 ,
	    0x3C0 , 0, BIT(0) , 0x3B8, 24 ,
	    0, 0, 0 , 0x3B8, 0, 22 ),
	PLL(CLK_APMIXED_MFGPLL, "mfgpll", 0x268 , 0x268, 0, 0 ,
	    0x274 , 0, BIT(0) , 0x26C, 24 ,
	    0, 0, 0 , 0x26C, 0, 22 ),
	PLL(CLK_APMIXED_TVDPLL, "tvdpll", 0x380 , 0x380, 0, 0 ,
	    0x38C , 0, BIT(0) , 0x384, 24 ,
	    0, 0, 0 , 0x384, 0, 22 ),
	PLL(CLK_APMIXED_APLL1, "apll1", 0x318 , 0x318, 0, 0 ,
	    0x328 , 0, BIT(0) , 0x31C, 24 ,
	    0x040, 0x00C, 0 , 0x320, 0, 32 ),
	PLL(CLK_APMIXED_APLL2, "apll2", 0x32C , 0x32C, 0, 0 ,
	    0x33C , 0, BIT(0) , 0x330, 24 ,
	    0x044, 0x00C, 5 , 0x334, 0, 32 ),
	PLL(CLK_APMIXED_USBPLL, "usbpll", 0x3C4 , 0x3CC, 0, 2 ,
	    0x3CC , HAVE_RST_BAR, BIT(23) , 0x3C4, 24 ,
	    0, 0, 0 ,0x3C4, 0, 22 ),
};

static const struct of_device_id of_match_clk_mt6789_apmixed[] = {
	{ .compatible = "mediatek,mt6789-apmixedsys" },
	{ /* sentinel */ }
};

static int clk_mt6789_apmixed_probe(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data;
	struct device_node *node = pdev->dev.of_node;
	int r;

	clk_data = mtk_alloc_clk_data(ARRAY_SIZE(plls));
	if (!clk_data)
		return -ENOMEM;

	r = mtk_clk_register_plls(node, plls, ARRAY_SIZE(plls), clk_data);
	if (r)
		goto free_apmixed_data;

	r = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, clk_data);
	if (r)
		goto unregister_plls;

	return r;

unregister_plls:
	mtk_clk_unregister_plls(plls, ARRAY_SIZE(plls), clk_data);
free_apmixed_data:
	mtk_free_clk_data(clk_data);
	return r;
}

static struct platform_driver clk_mt6789_apmixed_drv = {
	.probe = clk_mt6789_apmixed_probe,
	.driver = {
		.name = "clk-mt6789-apmixed",
		.of_match_table = of_match_clk_mt6789_apmixed,
	},
};
builtin_platform_driver(clk_mt6789_apmixed_drv);
MODULE_LICENSE("GPL");
