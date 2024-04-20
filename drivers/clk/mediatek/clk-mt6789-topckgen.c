// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include "clk-mtk.h"
#include "clk-gate.h"
#include "clk-mux.h"

#include <dt-bindings/clock/mt6789-clk.h>
#include <linux/clk.h>

static DEFINE_SPINLOCK(mt6789_clk_lock);

static const struct mtk_fixed_factor top_divs[] = {
	FACTOR(CLK_TOP_MFGPLL, "mfgpll_ck", "mfgpll", 1, 1),
	FACTOR(CLK_TOP_MAINPLL_D4, "mainpll_d4","mainpll", 1, 4),
	FACTOR(CLK_TOP_MAINPLL_D4_D2, "mainpll_d4_d2","mainpll", 1, 8),
	FACTOR(CLK_TOP_MAINPLL_D4_D4, "mainpll_d4_d4", "mainpll", 1, 16),
	FACTOR(CLK_TOP_MAINPLL_D4_D8, "mainpll_d4_d8", "mainpll", 1, 32),
	FACTOR(CLK_TOP_MAINPLL_D4_D16, "mainpll_d4_d16", "mainpll", 1, 64),
	FACTOR(CLK_TOP_MAINPLL_D5, "mainpll_d5", "mainpll", 1, 5),
	FACTOR(CLK_TOP_MAINPLL_D5_D2, "mainpll_d5_d2", "mainpll", 1, 10),
	FACTOR(CLK_TOP_MAINPLL_D5_D4, "mainpll_d5_d4", "mainpll", 1, 20),
	FACTOR(CLK_TOP_MAINPLL_D5_D8, "mainpll_d5_d8", "mainpll", 1, 40),
	FACTOR(CLK_TOP_MAINPLL_D6, "mainpll_d6", "mainpll", 1, 6),
	FACTOR(CLK_TOP_MAINPLL_D6_D2, "mainpll_d6_d2", "mainpll", 1, 12),
	FACTOR(CLK_TOP_MAINPLL_D6_D4, "mainpll_d6_d4", "mainpll", 1, 24),
	FACTOR(CLK_TOP_MAINPLL_D7, "mainpll_d7", "mainpll", 1, 7),
	FACTOR(CLK_TOP_MAINPLL_D7_D2, "mainpll_d7_d2", "mainpll", 1, 14),
	FACTOR(CLK_TOP_MAINPLL_D7_D4, "mainpll_d7_d4", "mainpll", 1, 28),
	FACTOR(CLK_TOP_MAINPLL_D7_D8, "mainpll_d7_d8", "mainpll", 1, 56),
	FACTOR(CLK_TOP_MAINPLL_D9, "mainpll_d9", "mainpll", 1, 9),
	FACTOR(CLK_TOP_UNIVPLL_D4, "univpll_d4", "univpll", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D4_D2, "univpll_d4_d2", "univpll", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D4_D4, "univpll_d4_d4", "univpll", 1, 16),
	FACTOR(CLK_TOP_UNIVPLL_D4_D8, "univpll_d4_d8", "univpll", 1, 32),
	FACTOR(CLK_TOP_UNIVPLL_D5, "univpll_d5", "univpll", 1, 5),
	FACTOR(CLK_TOP_UNIVPLL_D5_D2, "univpll_d5_d2", "univpll", 1, 10),
	FACTOR(CLK_TOP_UNIVPLL_D5_D4, "univpll_d5_d4", "univpll", 1, 20),
	FACTOR(CLK_TOP_UNIVPLL_D6, "univpll_d6", "univpll", 1, 6),
	FACTOR(CLK_TOP_UNIVPLL_D6_D2, "univpll_d6_d2", "univpll", 1, 12),
	FACTOR(CLK_TOP_UNIVPLL_D6_D4, "univpll_d6_d4", "univpll", 1, 24),
	FACTOR(CLK_TOP_UNIVPLL_D6_D8, "univpll_d6_d8", "univpll", 1, 48),
	FACTOR(CLK_TOP_UNIVPLL_D6_D16, "univpll_d6_d16", "univpll", 1, 96),
	FACTOR(CLK_TOP_UNIVPLL_D7, "univpll_d7", "univpll", 1, 7),
	FACTOR(CLK_TOP_UNIVPLL_192M_D2, "univpll_192m_d2", "univpll", 1, 26),
	FACTOR(CLK_TOP_UNIVPLL_192M_D4, "univpll_192m_d4", "univpll", 1, 52),
	FACTOR(CLK_TOP_UNIVPLL_192M_D8, "univpll_192m_d8", "univpll", 1, 104),
	FACTOR(CLK_TOP_UNIVPLL_192M_D16, "univpll_192m_d16", "univpll", 1, 208),
	FACTOR(CLK_TOP_UNIVPLL_192M_D32, "univpll_192m_d32", "univpll", 1, 416),
	FACTOR(CLK_TOP_APLL1, "apll1_ck", "apll1", 1, 1),
	FACTOR(CLK_TOP_APLL1_D2, "apll1_d2", "apll1", 1, 2),
	FACTOR(CLK_TOP_APLL1_D4, "apll1_d4", "apll1", 1, 4),
	FACTOR(CLK_TOP_APLL1_D8, "apll1_d8", "apll1", 1, 8),
	FACTOR(CLK_TOP_APLL2, "apll2_ck", "apll2", 1, 1),
	FACTOR(CLK_TOP_APLL2_D2, "apll2_d2", "apll2", 1, 2),
	FACTOR(CLK_TOP_APLL2_D4, "apll2_d4", "apll2", 1, 4),
	FACTOR(CLK_TOP_APLL2_D8, "apll2_d8", "apll2", 1, 8),
	FACTOR(CLK_TOP_MMPLL_D4_D2, "mmpll_d4_d2", "mmpll", 1, 8),
	FACTOR(CLK_TOP_MMPLL_D5_D2, "mmpll_d5_d2", "mmpll", 1, 12),
	FACTOR(CLK_TOP_MMPLL_D6, "mmpll_d6", "mmpll", 1, 6),
	FACTOR(CLK_TOP_MMPLL_D6_D2, "mmpll_d6_d2", "mmpll", 1, 12),
	FACTOR(CLK_TOP_MMPLL_D7, "mmpll_d7", "mmpll", 1, 7),
	FACTOR(CLK_TOP_MMPLL_D9, "mmpll_d9", "mmpll", 1, 9),
	FACTOR(CLK_TOP_NPUPLL, "npupll_ck", "npupll", 1, 1),
	FACTOR(CLK_TOP_TVDPLL, "tvdpll_ck", "tvdpll", 1, 1),
	FACTOR(CLK_TOP_MSDCPLL, "msdcpll_ck", "msdcpll", 1, 1),
	FACTOR(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", "msdcpll", 1, 2),
	FACTOR(CLK_TOP_MSDCPLL_D4, "msdcpll_d4", "msdcpll", 1, 4),
	FACTOR(CLK_TOP_CLKRTC, "clkrtc", "clk32k", 1, 1),
	FACTOR(CLK_TOP_TCK_26M_MX9, "tck_26m_mx9_ck", "clk26m", 1, 1),
	FACTOR(CLK_TOP_F26M_CK_D2, "f26m_d2", "clk26m", 1, 2),
	FACTOR(CLK_TOP_OSC_D2, "osc_d2", "ulposc", 1, 2),
	FACTOR(CLK_TOP_OSC_D4, "osc_d4", "ulposc", 1, 4),
	FACTOR(CLK_TOP_OSC_D8, "osc_d8", "ulposc", 1, 8),
	FACTOR(CLK_TOP_OSC_D16, "osc_d16", "ulposc", 1, 16),
	FACTOR(CLK_TOP_OSC_D10, "osc_d10", "ulposc", 1, 10),
	FACTOR(CLK_TOP_OSC_D20, "osc_d20", "ulposc", 1, 20),
	FACTOR(CLK_TOP_F26M, "f26m_ck", "clk26m", 1, 1),
	FACTOR(CLK_TOP_AXI, "axi_ck", "axi_sel", 1, 1),
	FACTOR(CLK_TOP_DISP, "disp_ck", "disp_sel", 1, 1),
	FACTOR(CLK_TOP_MDP, "mdp_ck", "mdp_sel", 1, 1),
	FACTOR(CLK_TOP_IMG1, "img1_ck", "img1_sel", 1, 1),
	FACTOR(CLK_TOP_IPE, "ipe_ck", "ipe_sel", 1, 1),
	FACTOR(CLK_TOP_CAM, "cam_ck", "cam_sel", 1, 1),
	FACTOR(CLK_TOP_MFG_REF, "mfg_ref_ck", "mfg_ref_sel", 1, 1),
	FACTOR(CLK_TOP_MFG_PLL, "mfg_pll_ck", "mfg_pll_sel", 1, 1),
	FACTOR(CLK_TOP_UART, "uart_ck", "uart_sel", 1, 1),
	FACTOR(CLK_TOP_SPI, "spi_ck", "spi_sel", 1, 1),
	FACTOR(CLK_TOP_MSDC50_0, "msdc50_0_ck", "msdc50_0_sel", 1, 1),
	FACTOR(CLK_TOP_MSDC30_1, "msdc30_1_ck", "msdc30_1_sel", 1, 1),
	FACTOR(CLK_TOP_AUDIO, "audio_ck", "audio_sel", 1, 1),
	FACTOR(CLK_TOP_PWRAP_ULPOSC, "pwrap_ulposc_ck", "pwrap_ulposc_sel", 1, 1),
	FACTOR(CLK_TOP_DISP_PWM, "disp_pwm_ck", "disp_pwm_sel", 1, 1),
	FACTOR(CLK_TOP_USB_TOP, "usb_ck", "usb_sel", 1, 1),
	FACTOR(CLK_TOP_I2C, "i2c_ck", "i2c_sel", 1, 1),
	FACTOR(CLK_TOP_AUD_ENGEN1, "aud_engen1_ck", "aud_engen1_sel", 1, 1),
	FACTOR(CLK_TOP_AUD_ENGEN2, "aud_engen2_ck", "aud_engen2_sel", 1, 1),
	FACTOR(CLK_TOP_AES_UFSFDE, "aes_ufsfde_ck", "aes_ufsfde_sel", 1, 1),
	FACTOR(CLK_TOP_UFS, "ufs_ck", "ufs_sel", 1, 1),
	FACTOR(CLK_TOP_DPMAIF_MAIN, "dpmaif_main_ck", "dpmaif_main_sel", 1, 1),
	FACTOR(CLK_TOP_VENC, "venc_ck", "venc_sel", 1, 1),
	FACTOR(CLK_TOP_VDEC, "vdec_ck", "vdec_sel", 1, 1),
	FACTOR(CLK_TOP_CAMTM, "camtm_ck", "camtm_sel", 1, 1),
	FACTOR(CLK_TOP_PWM, "pwm_ck", "pwm_sel", 1, 1),
	FACTOR(CLK_TOP_AUDIO_H, "audio_h_ck", "audio_h_sel", 1, 1),
	FACTOR(CLK_TOP_DSI_OCC, "dsi_occ_ck", "dsi_occ_sel", 1, 1),
	FACTOR(CLK_TOP_I2C_PSEUDO, "i2c_pseudo_ck", "ifrao_i2c_pseudo", 1, 1),
	FACTOR(CLK_TOP_APDMA, "apdma_ck", "ifrao_apdma_pseudo", 1, 1),
};

static const char * const axi_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d4",
	"mainpll_d7_d2",
	"mainpll_d4_d2",
	"mainpll_d5_d2",
	"mainpll_d6_d2",
	"osc_d4"
};

static const char * const spm_parents[] = {
	"tck_26m_mx9_ck",
	"osc_d10",
	"mainpll_d7_d4",
	"clkrtc"
};

static const char * const scp_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4",
	"npupll_ck",
	"mainpll_d6",
	"univpll_d6",
	"mainpll_d4_d2",
	"mainpll_d4",
	"mainpll_d7"
};

static const char * const bus_aximem_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d7_d2",
	"mainpll_d4_d2",
	"mainpll_d5_d2",
	"mainpll_d6"
};

static const char * const disp_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d6_d2",
	"mainpll_d5_d2",
	"mmpll_d6_d2",
	"univpll_d5_d2",
	"univpll_d4_d2",
	"mmpll_d7",
	"univpll_d6",
	"mainpll_d4",
	"mmpll_d5_d2"
};

static const char * const mdp_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d5_d2",
	"mmpll_d6_d2",
	"mainpll_d4_d2",
	"mmpll_d4_d2",
	"mainpll_d6",
	"mainpll_d5",
	"mainpll_d4",
	"tvdpll_ck",
	"univpll_d4",
	"mmpll_d5_d2"
};

static const char * const img1_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4",
	"tvdpll_ck",
	"mainpll_d4",
	"univpll_d5",
	"mmpll_d6",
	"univpll_d6",
	"mainpll_d6",
	"mmpll_d4_d2",
	"mainpll_d4_d2",
	"mmpll_d6_d2",
	"mmpll_d5_d2"
};

static const char * const ipe_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4",
	"mmpll_d6",
	"univpll_d6",
	"mainpll_d6",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"mmpll_d6_d2",
	"mmpll_d5_d2"
};

static const char * const cam_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4",
	"mmpll_d6",
	"univpll_d4",
	"univpll_d5",
	"univpll_d6",
	"mmpll_d7",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"npupll_ck"
};

static const char * const mfg_ref_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d6_d2",
	"mainpll_d6",
	"mainpll_d5_d2"
};

static const char * const mfg_pll_parents[] = {
	"mfg_ref_ck",
	"mfgpll_ck"
};

static const char * const camtg_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const camtg2_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const camtg3_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const camtg4_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const camtg5_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const camtg6_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d8",
	"univpll_d6_d8",
	"univpll_192m_d4",
	"univpll_d6_d16",
	"f26m_d2",
	"univpll_192m_d16",
	"univpll_192m_d32"
};

static const char * const uart_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d6_d8"
};

static const char * const spi_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d5_d4",
	"mainpll_d6_d4",
	"msdcpll_d4",
	"msdcpll_d2",
	"mainpll_d6_d2",
	"mainpll_d4_d4",
	"univpll_d5_d4"
};

static const char * const msdc5hclk_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d2",
	"mainpll_d6_d2"
};

static const char * const msdc50_0_parents[] = {
	"tck_26m_mx9_ck",
	"msdcpll_ck",
	"msdcpll_d2",
	"univpll_d4_d4",
	"mainpll_d6_d2",
	"univpll_d4_d2"
};

static const char * const msdc30_1_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d6_d2",
	"mainpll_d6_d2",
	"mainpll_d7_d2",
	"msdcpll_d2"
};

static const char * const audio_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d5_d8",
	"mainpll_d7_d8",
	"mainpll_d4_d16"
};

static const char * const aud_intbus_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d4",
	"mainpll_d7_d4"
};

static const char * const pwrap_ulposc_parents[] = {
	"osc_d10",
	"tck_26m_mx9_ck",
	"osc_d4",
	"osc_d8",
	"osc_d16"
};

static const char * const atb_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d2",
	"mainpll_d5_d2"
};

static const char * const sspm_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d7_d2",
	"mainpll_d6_d2",
	"mainpll_d5_d2",
	"mainpll_d9",
	"mainpll_d4_d2"
};

static const char * const scam_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d5_d4"
};

static const char * const disp_pwm_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d6_d4",
	"osc_d2",
	"osc_d4",
	"osc_d16"
};

static const char * const usb_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d5_d4",
	"univpll_d6_d4",
	"univpll_d5_d2"
};

static const char * const i2c_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d8",
	"univpll_d5_d4"
};

static const char * const seninf_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d4",
	"univpll_d6_d2",
	"univpll_d4_d2",
	"npupll_ck",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d5"
};

static const char * const seninf1_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d4",
	"univpll_d6_d2",
	"univpll_d4_d2",
	"npupll_ck",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d5"
};

static const char * const seninf2_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d4",
	"univpll_d6_d2",
	"univpll_d4_d2",
	"npupll_ck",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d5"
};

static const char * const seninf3_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d4",
	"univpll_d6_d2",
	"univpll_d4_d2",
	"npupll_ck",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d5"
};

static const char * const dxcc_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d2",
	"mainpll_d4_d4",
	"mainpll_d4_d8"
};

static const char * const aud_engen1_parents[] = {
	"tck_26m_mx9_ck",
	"apll1_d2",
	"apll1_d4",
	"apll1_d8"
};

static const char * const aud_engen2_parents[] = {
	"tck_26m_mx9_ck",
	"apll2_d2",
	"apll2_d4",
	"apll2_d8"
};

static const char * const aes_ufsfde_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mainpll_d4_d4",
	"univpll_d4_d2",
	"univpll_d6"
};

static const char * const ufs_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d4",
	"mainpll_d4_d8",
	"univpll_d4_d4",
	"mainpll_d6_d2",
	"mainpll_d5_d2",
	"msdcpll_d2"
};

static const char * const aud_1_parents[] = {
	"tck_26m_mx9_ck",
	"apll1_ck"
};

static const char * const aud_2_parents[] = {
	"tck_26m_mx9_ck",
	"apll2_ck"
};

static const char * const dpmaif_main_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d4",
	"mainpll_d6",
	"mainpll_d4_d2",
	"univpll_d4_d2"
};

static const char * const venc_parents[] = {
	"tck_26m_mx9_ck",
	"mmpll_d7",
	"mainpll_d6",
	"univpll_d4_d2",
	"mainpll_d4_d2",
	"univpll_d6",
	"mmpll_d6",
	"mainpll_d5_d2",
	"mainpll_d6_d2",
	"mmpll_d9",
	"univpll_d4_d4",
	"mainpll_d4",
	"univpll_d4",
	"univpll_d5",
	"univpll_d5_d2",
	"mainpll_d5"
};

static const char * const vdec_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_192m_d2",
	"univpll_d5_d4",
	"mainpll_d5",
	"mainpll_d5_d2",
	"mmpll_d6_d2",
	"univpll_d5_d2",
	"mainpll_d4_d2",
	"univpll_d4_d2",
	"univpll_d7",
	"mmpll_d7",
	"mmpll_d6",
	"univpll_d5",
	"mainpll_d4",
	"univpll_d4",
	"univpll_d6"
};

static const char * const camtm_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d7",
	"univpll_d6_d2",
	"univpll_d4_d2"
};

static const char * const pwm_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d4_d8"
};

static const char * const audio_h_parents[] = {
	"tck_26m_mx9_ck",
	"univpll_d7",
	"apll1_ck",
	"apll2_ck"
};

static const char * const spmi_mst_parents[] = {
	"tck_26m_mx9_ck",
	"f26m_d2",
	"osc_d8",
	"osc_d10",
	"osc_d16",
	"osc_d20",
	"clkrtc"
};

static const char * const dvfsrc_parents[] = {
	"tck_26m_mx9_ck",
	"osc_d10"
};

static const char * const aes_msdcfde_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d4_d2",
	"mainpll_d6",
	"mainpll_d4_d4",
	"univpll_d4_d2",
	"univpll_d6"
};

static const char * const mcupm_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d6_d4",
	"mainpll_d6_d2"
};

static const char * const dsi_occ_parents[] = {
	"tck_26m_mx9_ck",
	"mainpll_d6_d2",
	"univpll_d5_d2",
	"univpll_d4_d2"
};

static const char * const apll_i2s0_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s1_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s2_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s3_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s4_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s5_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s6_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s7_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s8_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const char * const apll_i2s9_mck_parents[] = {
	"aud_1_sel",
	"aud_2_sel"
};

static const struct mtk_mux top_muxes[] = {
	/* CLK_CFG_0 */
	MUX_CLR_SET_UPD(CLK_TOP_AXI_SEL , "axi_sel", axi_parents , 0x10, 0x14,
			0x18 , 0 , 3 ,0x04 , 0 ),
	MUX_CLR_SET_UPD(CLK_TOP_SPM_SEL , "spm_sel", spm_parents , 0x10, 0x14,
			0x18 , 8 , 2 , 0x04 , 1 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SCP_SEL , "scp_sel", scp_parents , 0x10, 0x14,
			     0x18 , 16 , 3 , 23 , 0x04 , 2 ),
	MUX_CLR_SET_UPD(CLK_TOP_BUS_AXIMEM_SEL , "bus_aximem_sel", bus_aximem_parents , 0x10, 0x14,
			0x18 , 24 , 3 , 0x04 , 3 ),
	/* CLK_CFG_1 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DISP_SEL , "disp_sel", disp_parents , 0x20, 0x24,
			     0x28 , 0 , 4 , 7 , 0x04 , 4 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MDP_SEL , "mdp_sel", mdp_parents , 0x20, 0x24,
			     0x28 , 8 , 4 , 15 , 0x04 ,5 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_IMG1_SEL , "img1_sel", img1_parents , 0x20, 0x24,
			     0x28 , 16 , 4 , 23 , 0x04 , 6 ),
	/* CLK_CFG_2 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_IPE_SEL , "ipe_sel", ipe_parents , 0x30, 0x34,
			     0x38 , 0 , 4 , 7 , 0x04 , 8 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAM_SEL , "cam_sel", cam_parents , 0x30, 0x34,
			     0x38 , 16 , 4 , 23 , 0x04 , 10 ),
	/* CLK_CFG_4 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MFG_REF_SEL , "mfg_ref_sel", mfg_ref_parents , 0x50, 0x54,
			     0x58 , 16 , 2 , 23 , 0x04 , 18 ),
	MUX_CLR_SET_UPD(CLK_TOP_MFG_PLL_SEL , "mfg_pll_sel", mfg_pll_parents , 0x50, 0x54,
			0x58 , 18 , 1 , INV_OFS , INV_BIT ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG_SEL , "camtg_sel", camtg_parents , 0x50, 0x54,
			     0x58 , 24 , 3 , 31 , 0x04 , 19 ),
	/* CLK_CFG_5 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG2_SEL , "camtg2_sel", camtg2_parents , 0x60, 0x64,
			     0x68 , 0 , 3 , 7 , 0x04 , 20 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG3_SEL , "camtg3_sel", camtg3_parents , 0x60, 0x64,
			     0x68 , 8 , 3 , 15 , 0x04 , 21 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG4_SEL , "camtg4_sel", camtg4_parents , 0x60, 0x64,
			     0x68 , 16 , 3 , 23 , 0x04 ,22 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG5_SEL , "camtg5_sel", camtg5_parents , 0x60, 0x64,
			     0x68 , 24 , 3 , 31 , 0x04 , 23 ),
	/* CLK_CFG_6 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG6_SEL , "camtg6_sel", camtg6_parents , 0x70, 0x74,
			     0x78 , 0 , 3 , 7 , 0x04 , 24 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_UART_SEL , "uart_sel", uart_parents , 0x70, 0x74,
			     0x78 , 8 , 1 , 15 , 0x04 , 25 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SPI_SEL , "spi_sel", spi_parents , 0x70, 0x74,
			     0x78 , 16 , 3 , 23 , 0x04 , 26 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC50_0_HCLK_SEL , "msdc5hclk_sel", msdc5hclk_parents , 0x70, 
			     0x74, 0x78 , 24 , 2 , 31 , 0x04 , 27 ),
	/* CLK_CFG_7 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC50_0_SEL , "msdc50_0_sel", msdc50_0_parents , 0x80, 0x84,
			     0x88 , 0 , 3 , 7 , 0x04 , 28 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC30_1_SEL , "msdc30_1_sel", msdc30_1_parents , 0x80, 0x84,
			     0x88 , 8 , 3 ,15 , 0x04 , 29 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUDIO_SEL , "audio_sel", audio_parents , 0x80, 0x84,
			     0x88 , 24 , 2 , 31 , 0x08 , 0 ),
	/* CLK_CFG_8 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_INTBUS_SEL , "aud_intbus_sel", aud_intbus_parents , 0x90, 
			     0x94, 0x98 , 0 , 2 , 7 , 0x08 , 1 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_PWRAP_ULPOSC_SEL , "pwrap_ulposc_sel", pwrap_ulposc_parents ,
			     0x90, 0x94, 0x98 , 8 , 3 , 15 , 0x08 , 2 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_ATB_SEL , "atb_sel", atb_parents , 0x90, 0x94,
			     0x98 , 16 , 2 , 23 , 0x08 , 3 ),
	MUX_CLR_SET_UPD(CLK_TOP_SSPM_SEL , "sspm_sel", sspm_parents , 0x90, 0x94,
			     0x98 , 24 , 3 , 0x08 , 4 ),
	/* CLK_CFG_9 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SCAM_SEL , "scam_sel", scam_parents , 0xA0, 0xA4,
			     0xA8 , 8 , 1 , 15 , 0x08 , 6 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DISP_PWM_SEL , "disp_pwm_sel", disp_pwm_parents , 0xA0, 0xA4,
			     0xA8 , 16 , 3 , 23 , 0x08 , 7 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_USB_TOP_SEL , "usb_sel", usb_parents , 0xA0, 0xA4,
			     0xA8 , 24 , 2 , 31 , 0x08 , 8 ),
	/* CLK_CFG_10 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_I2C_SEL , "i2c_sel", i2c_parents , 0xB0, 0xB4,
			     0xB8 , 8 , 2 , 15 , 0x08 , 10 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SENINF_SEL , "seninf_sel", seninf_parents , 0xB0, 0xB4,
			     0xB8 , 16 , 3 , 23 , 0x08 , 11 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SENINF1_SEL , "seninf1_sel", seninf1_parents , 0xB0, 0xB4,
			     0xB8 , 24 , 3 , 31 , 0x08 , 12 ),
	/* CLK_CFG_11 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SENINF2_SEL , "seninf2_sel", seninf2_parents , 0xC0, 0xC4,
			     0xC8 , 0 , 3 , 7 , 0x08 , 13 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SENINF3_SEL , "seninf3_sel", seninf3_parents , 0xC0, 0xC4,
			     0xC8 , 8 , 3 , 15 , 0x08 , 14 ),
	MUX_CLR_SET_UPD(CLK_TOP_DXCC_SEL , "dxcc_sel", dxcc_parents , 0xC0, 0xC4,
			     0xC8 , 24 , 2 , 0x08 , 16 ),
	/* CLK_CFG_12 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_ENGEN1_SEL , "aud_engen1_sel", aud_engen1_parents , 0xD0,
			     0xD4, 0xD8 , 0 , 2 , 7 , 0x08 , 17 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_ENGEN2_SEL , "aud_engen2_sel", aud_engen2_parents , 0xD0,
			     0xD4, 0xD8 , 8 , 2 , 15 , 0x08 , 18 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AES_UFSFDE_SEL , "aes_ufsfde_sel", aes_ufsfde_parents , 0xD0,
			     0xD4, 0xD8 , 16 , 3 , 23 , 0x08 , 19 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_UFS_SEL , "ufs_sel", ufs_parents , 0xD0, 0xD4,
			     0xD8 , 24 , 3 , 31 , 0x08 , 20 ),
	/* CLK_CFG_13 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_1_SEL , "aud_1_sel", aud_1_parents , 0xE0, 0xE4,
			     0xE8 , 0 , 1 , 7 , 0x08 , 21 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_2_SEL , "aud_2_sel", aud_2_parents , 0xE0, 0xE4,
			     0xE8 , 8 , 1 , 15 , 0x08 , 22 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DPMAIF_MAIN_SEL , "dpmaif_main_sel", dpmaif_main_parents ,
			     0xE0, 0xE4, 0xE8 , 24 , 3 , 31 , 0x08 , 24 ),
	/* CLK_CFG_14 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_VENC_SEL , "venc_sel", venc_parents , 0xF0, 0xF4,
			     0xF8 , 0 , 4 , 7 , 0x08 , 25 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_VDEC_SEL , "vdec_sel", vdec_parents , 0xF0, 0xF4,
			     0xF8 , 8 , 4 , 15 , 0x08 , 26 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTM_SEL , "camtm_sel", camtm_parents , 0xF0, 0xF4,
			     0xF8 , 16 , 2 , 23 , 0x08 , 27 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_PWM_SEL , "pwm_sel", pwm_parents , 0xF0, 0xF4,
			     0xF8 , 24 , 1 , 31 , 0x08 , 28 ),
	/* CLK_CFG_15 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUDIO_H_SEL , "audio_h_sel", audio_h_parents , 0x0100, 0x0104,
			     0x0108 , 0 , 2 , 7 , 0x08 , 29 ),
	MUX_CLR_SET_UPD(CLK_TOP_SPMI_MST_SEL , "spmi_mst_sel", spmi_mst_parents , 0x0100, 0x0104,
			0x0108 , 8 , 3 , 0x08 , 30 ),
	MUX_CLR_SET_UPD(CLK_TOP_DVFSRC_SEL , "dvfsrc_sel", dvfsrc_parents , 0x0100, 0x0104,
			0x0108 , 16 , 1 , 0x0c , 0 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AES_MSDCFDE_SEL , "aes_msdcfde_sel", aes_msdcfde_parents ,
			     0x0100, 0x0104, 0x0108 , 24 , 3 , 31 , 0x0c , 1 ),
	/* CLK_CFG_16 */
	MUX_CLR_SET_UPD(CLK_TOP_MCUPM_SEL , "mcupm_sel", mcupm_parents , 0x0110, 0x0114,
			0x0118 , 0 , 2 , 0x0c , 2 ),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DSI_OCC_SEL , "dsi_occ_sel", dsi_occ_parents , 0x0110, 0x0114,
			     0x0118 , 16 , 2 , 23 , 0x0c , 4 ),
};


static const struct mtk_composite top_aud_divs[] = {
	/* CLK_AUDDIV_0 */
	MUX(CLK_TOP_APLL_I2S0_MCK_SEL , "apll_i2s0_mck_sel",
	    apll_i2s0_mck_parents , 0x0320 ,16 , 1 ),
	MUX(CLK_TOP_APLL_I2S1_MCK_SEL , "apll_i2s1_mck_sel",
	    apll_i2s1_mck_parents , 0x0320 ,17 , 1 ),
	MUX(CLK_TOP_APLL_I2S2_MCK_SEL , "apll_i2s2_mck_sel",
	    apll_i2s2_mck_parents , 0x0320 ,18 , 1 ),
	MUX(CLK_TOP_APLL_I2S3_MCK_SEL , "apll_i2s3_mck_sel",
	    apll_i2s3_mck_parents , 0x0320 ,19 , 1 ),
	MUX(CLK_TOP_APLL_I2S4_MCK_SEL , "apll_i2s4_mck_sel",
	    apll_i2s4_mck_parents , 0x0320 ,20 , 1 ),
	MUX(CLK_TOP_APLL_I2S5_MCK_SEL , "apll_i2s5_mck_sel",
	    apll_i2s5_mck_parents , 0x0320 ,21 , 1 ),
	MUX(CLK_TOP_APLL_I2S6_MCK_SEL , "apll_i2s6_mck_sel",
	    apll_i2s6_mck_parents , 0x0320 ,22 , 1 ),
	MUX(CLK_TOP_APLL_I2S7_MCK_SEL , "apll_i2s7_mck_sel",
	    apll_i2s7_mck_parents , 0x0320 ,23 , 1 ),
	MUX(CLK_TOP_APLL_I2S8_MCK_SEL , "apll_i2s8_mck_sel",
	    apll_i2s8_mck_parents , 0x0320 ,24 , 1 ),
	MUX(CLK_TOP_APLL_I2S9_MCK_SEL , "apll_i2s9_mck_sel",
	    apll_i2s9_mck_parents , 0x0320 ,25 , 1 ),
	/* CLK_AUDDIV_2 */
	DIV_GATE(CLK_TOP_APLL12_CK_DIV0 , "apll12_div0" ,
		 "apll_i2s0_mck_sel" , 0x0320 ,0 , 0x0328 , 8 ,0 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV1 , "apll12_div1" ,
		 "apll_i2s1_mck_sel" , 0x0320 ,1 , 0x0328 , 8 ,8 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV2 , "apll12_div2" ,
		 "apll_i2s2_mck_sel" , 0x0320 ,2 , 0x0328 , 8 ,16 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV3 , "apll12_div3" ,
		 "apll_i2s3_mck_sel" , 0x0320 ,3 , 0x0328 , 8 ,24 ),
	/* CLK_AUDDIV_3 */
	DIV_GATE(CLK_TOP_APLL12_CK_DIV4 , "apll12_div4" ,
		 "apll_i2s4_mck_sel" , 0x0320 ,4 , 0x0334 , 8 ,0 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIVB , "apll12_divb" ,
		 "apll12_div4" , 0x0320 ,5 , 0x0334 , 8 ,8 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV5 , "apll12_div5" ,
		 "apll_i2s5_mck_sel" , 0x0320 ,6 , 0x0334 , 8 ,16 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV6 , "apll12_div6" ,
		 "apll_i2s6_mck_sel" , 0x0320 ,7 , 0x0334 , 8 ,24 ),
	/* CLK_AUDDIV_4 */
	DIV_GATE(CLK_TOP_APLL12_CK_DIV7 , "apll12_div7" ,
		 "apll_i2s7_mck_sel" , 0x0320 ,8 , 0x0338 , 8 ,0 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV8 , "apll12_div8" ,
		 "apll_i2s8_mck_sel" , 0x0320 ,9 , 0x0338 , 8 ,8 ),
	DIV_GATE(CLK_TOP_APLL12_CK_DIV9 , "apll12_div9" ,
		 "apll_i2s9_mck_sel" , 0x0320 ,10 , 0x0338 , 8 ,16 ),
	};

static const struct mtk_clk_desc topck_desc = {
	.factor_clks = top_divs,
	.num_factor_clks = ARRAY_SIZE(top_divs),
	.mux_clks = top_muxes,
	.num_mux_clks = ARRAY_SIZE(top_muxes),
	.composite_clks = top_aud_divs,
	.num_composite_clks = ARRAY_SIZE(top_aud_divs),
	.clk_lock = &mt6789_clk_lock,
};

static const struct of_device_id of_match_clk_mt6789_topckgen[] = {
	{ .compatible = "mediatek,mt6789-topckgen", .data = &topck_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6789_topckgen);

static struct platform_driver clk_mt6789_topckgen_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6789-topckgen",
		.of_match_table = of_match_clk_mt6789_topckgen,
	},
};
module_platform_driver(clk_mt6789_topckgen_drv);
MODULE_LICENSE("GPL");
