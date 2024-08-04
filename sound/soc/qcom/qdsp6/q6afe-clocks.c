// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2020, Linaro Limited

#include <dt-bindings/sound/qcom,q6afe.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include "q6dsp-lpass-clocks.h"
#include "q6afe.h"

struct q6dsp_clk_v1_init {
	int clk_id;
	int q6dsp_clk_id;
	char *name;
	int rate;
	int clk_root;
	int clk_src;
	int dir;
	struct q6afe_port *port;
};

#define Q6AFE_CLK(id) {					\
		.clk_id	= id,				\
		.q6dsp_clk_id	= Q6AFE_##id,		\
		.name = #id,				\
		.rate = 19200000,			\
	}

#define Q6AFE_CLK_DIG_V1(id) {					\
		.clk_id	= id,				\
		.clk_id	= LPAIF_DIG_CLK,		\
		.name = #id,				\
		.port=,					\
		.clk_src= 0,				\
		.clk_root= 5,				\
		.rate = 19200000,			\
	}

#define Q6AFE_CLK_BIT_V1(id,port) {					\
		.clk_id	= id,				\
		.clk_id	= LPAIF_BIT_CLK,		\
		.name = #id,				\
		.port=*q6afe_port_get_from_id,					\
		.clk_src=Q6AFE_LPASS_CLK_SRC_INTERNAL,				\
		.clk_root=Q6AFE_LPASS_CLK_ROOT_DEFAULT,				\
		.rate = 19200000,			\
	}
	
#define Q6AFE_CLK_OSR_V1(id) {					\
		.clk_id	= id,				\
		.clk_id	= LPAIF_OSR_CLK,		\
		.name = #id,				\
		.port=,					\
		.clk_src=Q6AFE_LPASS_CLK_SRC_INTERNAL,				\
		.clk_root=Q6AFE_LPASS_CLK_ROOT_DEFAULT,				\
		.rate = 19200000,			\
	}

static const struct q6dsp_clk_v1_init q6afe_clks[] = {
	Q6AFE_CLK_BIT_V1(LPASS_CLK_ID_PRI_MI2S_IBIT, AFE_PORT_ID_PRIMARY_MI2S_RX),
	Q6AFE_CLK_BIT_V1(LPASS_CLK_ID_SEC_MI2S_IBIT, AFE_PORT_ID_SECONDARY_MI2S_RX),
	Q6AFE_CLK_BIT_V1(LPASS_CLK_ID_TER_MI2S_IBIT, AFE_PORT_ID_TERTIARY_MI2S_RX),
	Q6AFE_CLK_BIT_V1(LPASS_CLK_ID_QUAD_MI2S_IBIT, AFE_PORT_ID_QUATERNARY_MI2S_RX),
	Q6AFE_CLK_BIT_V1(LPASS_CLK_ID_QUI_MI2S_IBIT, AFE_PORT_ID_QUINARY_MI2S_RX),		
	Q6AFE_CLK_DIG_V1(LPASS_CLK_ID_INTERNAL_DIGITAL_CODEC_CORE,AFE_PORT_ID_PRIMARY_MI2S_RX),
};

static const struct q6dsp_clk_init q6afe_clks[] = {
	Q6AFE_CLK(LPASS_CLK_ID_PRI_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_PRI_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SPEAKER_I2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SPEAKER_I2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SPEAKER_I2S_OSR),
	Q6AFE_CLK(LPASS_CLK_ID_QUI_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUI_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEN_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEN_MI2S_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT0_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT1_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT2_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT3_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT4_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT5_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_INT6_MI2S_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUI_MI2S_OSR),
	Q6AFE_CLK(LPASS_CLK_ID_PRI_PCM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_PRI_PCM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_PCM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_PCM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_PCM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_PCM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_PCM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_PCM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUIN_PCM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUIN_PCM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUI_PCM_OSR),
	Q6AFE_CLK(LPASS_CLK_ID_PRI_TDM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_PRI_TDM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_TDM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_SEC_TDM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_TDM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_TER_TDM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_TDM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUAD_TDM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUIN_TDM_IBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUIN_TDM_EBIT),
	Q6AFE_CLK(LPASS_CLK_ID_QUIN_TDM_OSR),
	Q6AFE_CLK(LPASS_CLK_ID_MCLK_1),
	Q6AFE_CLK(LPASS_CLK_ID_MCLK_2),
	Q6AFE_CLK(LPASS_CLK_ID_MCLK_3),
	Q6AFE_CLK(LPASS_CLK_ID_MCLK_4),
	Q6AFE_CLK(LPASS_CLK_ID_INTERNAL_DIGITAL_CODEC_CORE),
	Q6AFE_CLK(LPASS_CLK_ID_INT_MCLK_0),
	Q6AFE_CLK(LPASS_CLK_ID_INT_MCLK_1),
	Q6AFE_CLK(LPASS_CLK_ID_WSA_CORE_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_WSA_CORE_NPL_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_VA_CORE_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_TX_CORE_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_TX_CORE_NPL_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_RX_CORE_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_RX_CORE_NPL_MCLK),
	Q6AFE_CLK(LPASS_CLK_ID_VA_CORE_2X_MCLK),
	Q6DSP_VOTE_CLK(LPASS_HW_AVTIMER_VOTE,
		       Q6AFE_LPASS_CORE_AVTIMER_BLOCK,
		       "LPASS_AVTIMER_MACRO"),
	Q6DSP_VOTE_CLK(LPASS_HW_MACRO_VOTE,
		       Q6AFE_LPASS_CORE_HW_MACRO_BLOCK,
		       "LPASS_HW_MACRO"),
	Q6DSP_VOTE_CLK(LPASS_HW_DCODEC_VOTE,
		       Q6AFE_LPASS_CORE_HW_DCODEC_BLOCK,
		       "LPASS_HW_DCODEC"),
};

static const struct q6dsp_clk_desc q6dsp_clk_q6afe_v2 __maybe_unused = {
	.clks = q6afe_clks,
	.num_clks = ARRAY_SIZE(q6afe_clks),
	.lpass_set_clk = q6afe_set_lpass_clock,
	.lpass_vote_clk = q6afe_vote_lpass_core_hw,
	.lpass_unvote_clk = q6afe_unvote_lpass_core_hw,
};

static const struct q6dsp_clk_desc q6dsp_clk_q6afe_v1 __maybe_unused = {
	.clks = q6afe_clks_v1,
	.num_clks = ARRAY_SIZE(q6afe_clks_v1),
	.lpass_set_clk = q6afe_port_set_sysclk,
};

#ifdef CONFIG_OF
static const struct of_device_id q6afe_clock_device_id[] = {
	{ .compatible = "qcom,q6afe-clocks", .data = &q6dsp_clk_q6afe },
	//make q6afe-clocks discoverable based on AVS????
	{},
};
MODULE_DEVICE_TABLE(of, q6afe_clock_device_id);
#endif

static struct platform_driver q6afe_clock_platform_driver = {
	.driver = {
		.name = "q6afe-clock",
		.of_match_table = of_match_ptr(q6afe_clock_device_id),
	},
	.probe = q6dsp_clock_dev_probe,
};
module_platform_driver(q6afe_clock_platform_driver);

MODULE_DESCRIPTION("Q6 Audio Frontend clock driver");
MODULE_LICENSE("GPL v2");
