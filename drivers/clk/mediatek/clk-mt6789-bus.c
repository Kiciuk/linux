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

static const struct mtk_gate_regs ifrao0_cg_regs = {
	.set_ofs = 0x80,
	.clr_ofs = 0x84,
	.sta_ofs = 0x90,
};

static const struct mtk_gate_regs ifrao1_cg_regs = {
	.set_ofs = 0x88,
	.clr_ofs = 0x8C,
	.sta_ofs = 0x94,
};

static const struct mtk_gate_regs ifrao2_cg_regs = {
	.set_ofs = 0xA4,
	.clr_ofs = 0xA8,
	.sta_ofs = 0xAC,
};

static const struct mtk_gate_regs ifrao3_cg_regs = {
	.set_ofs = 0xC0,
	.clr_ofs = 0xC4,
	.sta_ofs = 0xC8,
};

static const struct mtk_gate_regs ifrao4_cg_regs = {
	.set_ofs = 0xE0,
	.clr_ofs = 0xE4,
	.sta_ofs = 0xE8,
};

static const struct mtk_gate_regs ifrao5_cg_regs = {
	.set_ofs = 0xd0,
	.clr_ofs = 0xd4,
	.sta_ofs = 0xd8,
};

#define GATE_IFRAO0(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao0_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_IFRAO1(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao1_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_IFRAO2(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao2_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_IFRAO3(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao3_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_IFRAO4(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao4_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_IFRAO5(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ifrao5_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate ifrao_clks[] = {
	/* IFRAO0 */
	GATE_IFRAO0(CLK_IFRAO_PMIC_TMR, "ifrao_pmic_tmr" , "pwrap_ulposc_ck" , 0),
	GATE_IFRAO0(CLK_IFRAO_PMIC_AP, "ifrao_pmic_ap" , "pwrap_ulposc_ck" , 1),
	GATE_IFRAO0(CLK_IFRAO_GCE, "ifrao_gce" , "axi_ck" , 8),
	GATE_IFRAO0(CLK_IFRAO_GCE2, "ifrao_gce2" , "axi_ck" , 9),
	GATE_IFRAO0(CLK_IFRAO_THERM, "ifrao_therm" , "axi_ck" , 10),
	GATE_IFRAO0(CLK_IFRAO_I2C_PSEUDO, "ifrao_i2c_pseudo" , "i2c_ck" , 11),
	GATE_IFRAO0(CLK_IFRAO_APDMA_PSEUDO, "ifrao_apdma_pseudo" , "axi_ck" , 12),
	GATE_IFRAO0(CLK_IFRAO_PWM_HCLK, "ifrao_pwm_hclk" , "axi_ck" , 15),
	GATE_IFRAO0(CLK_IFRAO_PWM1, "ifrao_pwm1" , "pwm_ck" , 16),
	GATE_IFRAO0(CLK_IFRAO_PWM2, "ifrao_pwm2" , "pwm_ck" , 17),
	GATE_IFRAO0(CLK_IFRAO_PWM3, "ifrao_pwm3" , "pwm_ck" , 18),
	GATE_IFRAO0(CLK_IFRAO_PWM4, "ifrao_pwm4" , "pwm_ck" , 19),
	GATE_IFRAO0(CLK_IFRAO_PWM, "ifrao_pwm" , "pwm_ck" , 21),
	GATE_IFRAO0(CLK_IFRAO_UART0, "ifrao_uart0" , "uart_ck" , 22),
	GATE_IFRAO0(CLK_IFRAO_UART1, "ifrao_uart1" , "uart_ck" , 23),
	GATE_IFRAO0(CLK_IFRAO_UART2, "ifrao_uart2" , "uart_ck" , 24),
	GATE_IFRAO0(CLK_IFRAO_UART3, "ifrao_uart3" , "uart_ck" , 25),
	GATE_IFRAO0(CLK_IFRAO_GCE_26M, "ifrao_gce_26m" , "axi_ck" , 27),
	GATE_IFRAO0(CLK_IFRAO_BTIF, "ifrao_btif" , "axi_ck" , 31),
	/* IFRAO1 */
	GATE_IFRAO1(CLK_IFRAO_SPI0, "ifrao_spi0" , "spi_ck" , 1),
	GATE_IFRAO1(CLK_IFRAO_MSDC0, "ifrao_msdc0" , "axi_ck" , 2),
	GATE_IFRAO1(CLK_IFRAO_MSDC1, "ifrao_msdc1" , "axi_ck" , 4),
	GATE_IFRAO1(CLK_IFRAO_MSDC0_SRC, "ifrao_msdc0_clk" , "msdc50_0_ck" , 6),
	GATE_IFRAO1(CLK_IFRAO_AUXADC, "ifrao_auxadc" , "f26m_ck" , 10),
	GATE_IFRAO1(CLK_IFRAO_CPUM, "ifrao_cpum" , "axi_ck" , 11),
	GATE_IFRAO1(CLK_IFRAO_CCIF1_AP, "ifrao_ccif1_ap" , "axi_ck" , 12),
	GATE_IFRAO1(CLK_IFRAO_CCIF1_MD, "ifrao_ccif1_md" , "axi_ck" , 13),
	GATE_IFRAO1(CLK_IFRAO_AUXADC_MD, "ifrao_auxadc_md" , "f26m_ck" , 14),
	GATE_IFRAO1(CLK_IFRAO_MSDC1_SRC, "ifrao_msdc1_clk" , "msdc30_1_ck" , 16),
	GATE_IFRAO1(CLK_IFRAO_MSDC0_AES, "ifrao_msdc0_aes_clk" , "msdc50_0_ck" , 17),
	GATE_IFRAO1(CLK_IFRAO_CCIF_AP, "ifrao_ccif_ap" , "axi_ck" , 23),
	GATE_IFRAO1(CLK_IFRAO_DEBUGSYS, "ifrao_debugsys" , "axi_ck" , 24),
	GATE_IFRAO1(CLK_IFRAO_AUDIO, "ifrao_audio" , "axi_ck" , 25),
	GATE_IFRAO1(CLK_IFRAO_CCIF_MD, "ifrao_ccif_md" , "axi_ck" , 26),
	/* IFRAO2 */
	GATE_IFRAO2(CLK_IFRAO_SSUSB, "ifrao_ssusb" , "usb_ck" , 1),
	GATE_IFRAO2(CLK_IFRAO_DISP_PWM, "ifrao_disp_pwm" , "disp_pwm_ck" , 2),
	GATE_IFRAO2(CLK_IFRAO_CLDMA_BCLK, "ifrao_cldmabclk" , "axi_ck" , 3),
	GATE_IFRAO2(CLK_IFRAO_AUDIO_26M_BCLK, "ifrao_audio26m" , "f26m_ck" , 4),
	GATE_IFRAO2(CLK_IFRAO_SPI1, "ifrao_spi1" , "spi_ck" , 6),
	GATE_IFRAO2(CLK_IFRAO_SPI2, "ifrao_spi2" , "spi_ck" , 9),
	GATE_IFRAO2(CLK_IFRAO_SPI3, "ifrao_spi3" , "spi_ck" , 10),
	GATE_IFRAO2(CLK_IFRAO_UNIPRO_SYSCLK, "ifrao_unipro_sysclk" , "ufs_ck" , 11),
	GATE_IFRAO2(CLK_IFRAO_UNIPRO_TICK, "ifrao_unipro_tick" , "f26m_ck" , 12),
	GATE_IFRAO2(CLK_IFRAO_UFS_SAP_BCLK, "ifrao_u_bclk" , "f26m_ck" , 13),
	GATE_IFRAO2(CLK_IFRAO_SPI4, "ifrao_spi4" , "spi_ck" , 25),
	GATE_IFRAO2(CLK_IFRAO_SPI5, "ifrao_spi5" , "spi_ck" , 26),
	GATE_IFRAO2(CLK_IFRAO_CQ_DMA, "ifrao_cq_dma" , "axi_ck" , 27),
	GATE_IFRAO2(CLK_IFRAO_UFS, "ifrao_ufs" , "ufs_ck" , 28),
	GATE_IFRAO2(CLK_IFRAO_UFS_AES, "ifrao_u_aes" , "aes_ufsfde_ck" , 29),
	/* IFRAO3 */
	GATE_IFRAO3(CLK_IFRAO_AP_MSDC0, "ifrao_ap_msdc0" , "msdc50_0_ck" , 7),
	GATE_IFRAO3(CLK_IFRAO_MD_MSDC0, "ifrao_md_msdc0" , "msdc50_0_ck" , 8),
	GATE_IFRAO3(CLK_IFRAO_CCIF5_MD, "ifrao_ccif5_md" , "axi_ck" , 10),
	GATE_IFRAO3(CLK_IFRAO_CCIF2_AP, "ifrao_ccif2_ap" , "axi_ck" , 16),
	GATE_IFRAO3(CLK_IFRAO_CCIF2_MD, "ifrao_ccif2_md" , "axi_ck" , 17),
	GATE_IFRAO3(CLK_IFRAO_FBIST2FPC, "ifrao_fbist2fpc" , "msdc50_0_ck" , 24),
	GATE_IFRAO3(CLK_IFRAO_DPMAIF_MAIN, "ifrao_dpmaif_main" , "dpmaif_main_ck" , 26),
	GATE_IFRAO3(CLK_IFRAO_CCIF4_AP, "ifrao_ccif4_ap" , "axi_ck" , 28),
	GATE_IFRAO3(CLK_IFRAO_CCIF4_MD, "ifrao_ccif4_md" , "axi_ck" , 29),
	GATE_IFRAO3(CLK_IFRAO_SPI6_CK, "ifrao_spi6_ck" , "spi_ck" , 30),
	GATE_IFRAO3(CLK_IFRAO_SPI7_CK, "ifrao_spi7_ck" , "spi_ck" , 31),
	/* IFRAO4 */
	GATE_IFRAO4(CLK_IFRAO_66MP_BUS_MCLK_CKP, "ifrao_66mp_mclkp" , "axi_ck" , 2),
	/* IFRAO5 */
	GATE_IFRAO5(CLK_IFRAO_AP_DMA, "ifrao_ap_dma" , "apdma_ck" , 31),
};

// ti,syscon reset definitions
static u16 ifrao_rst_ofs[] = {
	INFRA_RST0_SET_OFFSET, 
	INFRA_RST1_SET_OFFSET, 
	INFRA_RST2_SET_OFFSET, 
	INFRA_RST3_SET_OFFSET, 
};

static u16 ifrao_idx_map[] = {
	[MT6789_INFRA_RST0_LVTS_AP_SWRST] = 0 * RST_NR_PER_BANK + 0,
	[MT6789_INFRA_RST1_UFSHCI_SWRST] = 1 * RST_NR_PER_BANK + 15,
	[MT6789_INFRA_RST2_UNIPRO_SWRST] = 3 * RST_NR_PER_BANK + 7,
	[MT6789_INFRA_RST3_UFS_CRYPTO_SWRST] = 4 * RST_NR_PER_BANK + 21,
};

static const struct mtk_clk_rst_desc clk_rst_desc = {
	.version = MTK_RST_SET_CLR,
	.rst_bank_ofs = infrao_rst_ofs,
	.rst_bank_nr = ARRAY_SIZE(infrao_rst_ofs),
	.rst_idx_map = ifrao_idx_map,
	.rst_idx_map_nr = ARRAY_SIZE(infrao_idx_map),
};


static const struct mtk_clk_desc ifrao_mcd = {
	.clks = ifrao_clks,
	.num_clks = ARRAY_SIZE(ifrao_clks),
	.rst_desc = &clk_rst_desc,
};

static const struct of_device_id of_match_clk_mt6789_bus[] = {
	{
		.compatible = "mediatek,mt6789-infracfg_ao",
		.data = &ifrao_mcd,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_bus);

static struct platform_driver clk_mt6789_bus_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-bus",
		.of_match_table = of_match_clk_mt6789_bus,
	},
};

module_platform_driver(clk_mt6789_bus_drv);
MODULE_LICENSE("GPL");
