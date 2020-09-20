// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018-2020 Linaro Ltd
 * Author: Georgi Djakov <georgi.djakov@linaro.org>
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/interconnect-provider.h>
#include <linux/io.h>
#include <linux/regmap.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#include <dt-bindings/interconnect/qcom,msm8953.h>

#include "smd-rpm.h"

#define RPM_BUS_MASTER_REQ			0x73616d62
#define RPM_BUS_SLAVE_REQ			0x766c7362
#define RPM_SLV_ID(id)				.slv_rpm_id = (id + 1)
#define RPM_MAS_ID(id)				.mas_rpm_id = (id + 1)

#define BIMC_BKE_ENA_REG(qport)			(0x8300 + (qport) * 0x4000)
#define BIMC_BKE_ENA_MASK			GENMASK(1, 0)
#define BIMC_BKE_ENA_SHIFT			0
#define BIMC_BKE_HEALTH_REG(qport, hlvl)	(0x8340 + (qport) * 0x4000 \
						+ (hlvl) * 4)
#define BIMC_BKE_HEALTH_LIMIT_CMDS_MASK		GENMASK(31, 31)
#define BIMC_BKE_HEALTH_LIMIT_CMDS_SHIFT	31
#define BIMC_BKE_HEALTH_AREQPRIO_MASK		GENMASK(9, 8)
#define BIMC_BKE_HEALTH_AREQPRIO_SHIFT		8
#define BIMC_BKE_HEALTH_PRIOLVL_MASK		GENMASK(1, 0)
#define BIMC_BKE_HEALTH_PRIOLVL_SHIFT		0

#define NOC_QOS_PRIO_REG(qport)			(0x7008 + (qport) * 0x1000)
#define NOC_QOS_PRIO_P0_MASK			GENMASK(1, 0)
#define NOC_QOS_PRIO_P0_SHIFT			0
#define NOC_QOS_PRIO_P1_MASK			GENMASK(3, 2)
#define NOC_QOS_PRIO_P1_SHIFT			2

#define NOC_QOS_MODE_REG(qport)			(0x700c + (qport) * 0x1000)
#define NOC_QOS_MODE_MASK			GENMASK(1, 0)
#define NOC_QOS_MODE_SHIFT			0
#define NOC_QOS_MODE_FIXED			0
#define NOC_QOS_MODE_LIMITER			1
#define NOC_QOS_MODE_BYPASS			2
#define NOC_QOS_MODE_REGULATOR			3

#define MSM8953_MAX_LINKS			12

enum {
	MSM8953_MASTER_AMPSS_M0 = 1,
	MSM8953_MASTER_GRAPHICS_3D,
	MSM8953_SNOC_BIMC_0_MAS,
	MSM8953_SNOC_BIMC_2_MAS,
	MSM8953_SNOC_BIMC_1_MAS,
	MSM8953_MASTER_TCU_0,
	MSM8953_SLAVE_EBI_CH0,
	MSM8953_BIMC_SNOC_SLV,
	MSM8953_MASTER_SPDM,
	MSM8953_MASTER_BLSP_1,
	MSM8953_MASTER_BLSP_2,
	MSM8953_MASTER_USB3,
	MSM8953_MASTER_CRYPTO_CORE0,
	MSM8953_MASTER_SDCC_1,
	MSM8953_MASTER_SDCC_2,
	MSM8953_SNOC_PNOC_MAS,
	MSM8953_PNOC_M_0,
	MSM8953_PNOC_M_1,
	MSM8953_PNOC_INT_1,
	MSM8953_PNOC_INT_2,
	MSM8953_PNOC_SLV_0,
	MSM8953_PNOC_SLV_1,
	MSM8953_PNOC_SLV_2,
	MSM8953_PNOC_SLV_3,
	MSM8953_PNOC_SLV_4,
	MSM8953_PNOC_SLV_6,
	MSM8953_PNOC_SLV_7,
	MSM8953_PNOC_SLV_8,
	MSM8953_PNOC_SLV_9,
	MSM8953_SLAVE_SPDM_WRAPPER,
	MSM8953_SLAVE_PDM,
	MSM8953_SLAVE_TCSR,
	MSM8953_SLAVE_SNOC_CFG,
	MSM8953_SLAVE_TLMM,
	MSM8953_SLAVE_MESSAGE_RAM,
	MSM8953_SLAVE_BLSP_1,
	MSM8953_SLAVE_BLSP_2,
	MSM8953_SLAVE_PRNG,
	MSM8953_SLAVE_CAMERA_CFG,
	MSM8953_SLAVE_DISPLAY_CFG,
	MSM8953_SLAVE_VENUS_CFG,
	MSM8953_SLAVE_GRAPHICS_3D_CFG,
	MSM8953_SLAVE_SDCC_1,
	MSM8953_SLAVE_SDCC_2,
	MSM8953_SLAVE_CRYPTO_0_CFG,
	MSM8953_SLAVE_PMIC_ARB,
	MSM8953_SLAVE_USB3,
	MSM8953_SLAVE_IPA_CFG,
	MSM8953_SLAVE_TCU,
	MSM8953_PNOC_SNOC_SLV,
	MSM8953_MASTER_QDSS_BAM,
	MSM8953_BIMC_SNOC_MAS,
	MSM8953_PNOC_SNOC_MAS,
	MSM8953_MASTER_IPA,
	MSM8953_MASTER_QDSS_ETR,
	MSM8953_SNOC_QDSS_INT,
	MSM8953_SNOC_INT_0,
	MSM8953_SNOC_INT_1,
	MSM8953_SNOC_INT_2,
	MSM8953_SLAVE_APPSS,
	MSM8953_SLAVE_WCSS,
	MSM8953_SNOC_BIMC_1_SLV,
	MSM8953_SLAVE_OCIMEM,
	MSM8953_SNOC_PNOC_SLV,
	MSM8953_SLAVE_QDSS_STM,
	MSM8953_SLAVE_OCMEM_64,
	MSM8953_SLAVE_LPASS,
	MSM8953_MASTER_JPEG,
	MSM8953_MASTER_MDP_PORT0,
	MSM8953_MASTER_VIDEO_P0,
	MSM8953_MASTER_VFE,
	MSM8953_MASTER_VFE1,
	MSM8953_MASTER_CPP,
	MSM8953_SNOC_BIMC_0_SLV,
	MSM8953_SNOC_BIMC_2_SLV,
	MSM8953_SLAVE_CATS_128,
};

#define to_msm8953_provider(_provider) \
	container_of(_provider, struct msm8953_icc_provider, provider)

static const struct clk_bulk_data msm8953_bus_clocks[] = {
	{ .id = "bus" },
	{ .id = "bus_a" },
	{ .id = "mm_bus" },
	{ .id = "mm_bus_a" },
};

/**
 * struct msm8953_icc_provider - Qualcomm specific interconnect provider
 * @provider: generic interconnect provider
 * @bus_clks: the clk_bulk_data table of bus clocks
 * @num_clk_pairs: the total number of clk_bulk_data pairs
 */
struct msm8953_icc_provider {
	struct icc_provider provider;
	struct clk_bulk_data *bus_clks;
	int num_clk_pairs;
};

enum qos_mode {
	QOS_UNKNOWN,
	QOS_BYPASS,
	QOS_FIXED,
};

/**
 * struct msm8953_icc_node - Qualcomm specific interconnect nodes
 * @name: the node name used in debugfs
 * @id: a unique node identifier
 * @links: an array of nodes where we can go next while traversing
 * @num_links: the total number of @links
 * @qport: the offset index into the masters QoS register space
 * @ap_owned: if true then the AP will program the QoS registers for the device
 * @port0, @port1: priority low/high signal for NoC or prioity level for BIMC
 * @clk_sel: clock pair selector for device
 * @qos_mode: QoS mode to be programmed for this device.
 * @buswidth: width of the interconnect between a node and the bus (bytes)
 * @mas_rpm_id:	RPM ID for devices that are bus masters
 * @slv_rpm_id:	RPM ID for devices that are bus slaves
 * @rate: current bus clock rate in Hz
 */
struct msm8953_icc_node {
	unsigned char *name;
	u16 id;
	u16 links[MSM8953_MAX_LINKS];
	u16 num_links;
	bool ap_owned;
	u16 prio0;
	u16 prio1;
	u16 buswidth;
	u16 qport;
	u16 clk_sel;
	enum qos_mode qos_mode;
	int mas_rpm_id;
	int slv_rpm_id;
	u64 rate;
};

static void msm8953_bimc_node_init(struct msm8953_icc_node *qn, 
				  struct regmap* rmap);

static void msm8953_noc_node_init(struct msm8953_icc_node *qn, 
				  struct regmap* rmap);

#define QNODE(_name, _id, _qport, _ap_owned, _buswidth, _qos_mode,	\
	      _prio0, _prio1, _mas_rpm_id, _slv_rpm_id, ...)		\
		static struct msm8953_icc_node _name = {		\
		.name = #_name,						\
		.id = _id,						\
		.qport = _qport,					\
		.buswidth = _buswidth,					\
		.mas_rpm_id = _mas_rpm_id,				\
		.slv_rpm_id = _slv_rpm_id,				\
		.prio0 = _prio0,					\
		.prio1 = _prio1,					\
		.qos_mode = _qos_mode,					\
		.ap_owned = _ap_owned,					\
		.num_links = ARRAY_SIZE(((int[]){ __VA_ARGS__ })),	\
		.links = { __VA_ARGS__ },				\
	}

QNODE(mas_apps_proc, MSM8953_MASTER_AMPSS_M0, 0, 1, 8, QOS_FIXED, 0, 0, 0, -1,
	MSM8953_SLAVE_EBI_CH0, MSM8953_BIMC_SNOC_SLV);
QNODE(mas_oxili, MSM8953_MASTER_GRAPHICS_3D, 2, 1, 8, QOS_FIXED, 0, 0, 6, -1,
	MSM8953_SLAVE_EBI_CH0, MSM8953_BIMC_SNOC_SLV);
QNODE(mas_snoc_bimc_0, MSM8953_SNOC_BIMC_0_MAS, 3, 1, 8, QOS_BYPASS, 0, 0, 3, -1,
	MSM8953_SLAVE_EBI_CH0, MSM8953_BIMC_SNOC_SLV);
QNODE(mas_snoc_bimc_2, MSM8953_SNOC_BIMC_2_MAS, 4, 1, 8, QOS_BYPASS, 0, 0, 108, -1,
	MSM8953_SLAVE_EBI_CH0, MSM8953_BIMC_SNOC_SLV);
QNODE(mas_snoc_bimc_1, MSM8953_SNOC_BIMC_1_MAS, 5, 0, 8, QOS_BYPASS, 0, 0, 76, -1,
	MSM8953_SLAVE_EBI_CH0);
QNODE(mas_tcu_0, MSM8953_MASTER_TCU_0, 6, 1, 8, QOS_FIXED, 2, 2, 102, -1,
	MSM8953_SLAVE_EBI_CH0, MSM8953_BIMC_SNOC_SLV);
QNODE(slv_ebi, MSM8953_SLAVE_EBI_CH0, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 0, 0);
QNODE(slv_bimc_snoc, MSM8953_BIMC_SNOC_SLV, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 2,
	MSM8953_BIMC_SNOC_MAS);
QNODE(mas_spdm, MSM8953_MASTER_SPDM, 0, 1, 4, QOS_UNKNOWN, 0, 0, 50, -1,
	MSM8953_PNOC_M_0);
QNODE(mas_blsp_1, MSM8953_MASTER_BLSP_1, 0, 0, 4, QOS_UNKNOWN, 0, 0, 41, -1,
	MSM8953_PNOC_M_1);
QNODE(mas_blsp_2, MSM8953_MASTER_BLSP_2, 0, 0, 4, QOS_UNKNOWN, 0, 0, 39, -1,
	MSM8953_PNOC_M_1);
QNODE(mas_usb3, MSM8953_MASTER_USB3, 11, 1, 8, QOS_FIXED, 1, 1, 32, -1,
	MSM8953_PNOC_INT_1);
QNODE(mas_crypto, MSM8953_MASTER_CRYPTO_CORE0, 0, 1, 8, QOS_FIXED, 1, 1, 23, -1,
	MSM8953_PNOC_INT_1);
QNODE(mas_sdcc_1, MSM8953_MASTER_SDCC_1, 7, 0, 8, QOS_FIXED, 0, 0, 33, -1,
	MSM8953_PNOC_INT_1);
QNODE(mas_sdcc_2, MSM8953_MASTER_SDCC_2, 8, 0, 8, QOS_FIXED, 0, 0, 35, -1,
	MSM8953_PNOC_INT_1);
QNODE(mas_snoc_pcnoc, MSM8953_SNOC_PNOC_MAS, 9, 0, 8, QOS_FIXED, 0, 0, 77, -1,
	MSM8953_PNOC_INT_2);
QNODE(pcnoc_m_0, MSM8953_PNOC_M_0, 5, 1, 4, QOS_FIXED, 1, 1, 87, 116,
	MSM8953_PNOC_INT_1);
QNODE(pcnoc_m_1, MSM8953_PNOC_M_1, 6, 0, 4, QOS_FIXED, 0, 0, 88, 117,
	MSM8953_PNOC_INT_1);
QNODE(pcnoc_int_1, MSM8953_PNOC_INT_1, 0, 0, 8, QOS_UNKNOWN, 0, 0, 86, 115,
	MSM8953_PNOC_INT_2, MSM8953_PNOC_SNOC_SLV);
QNODE(pcnoc_int_2, MSM8953_PNOC_INT_2, 0, 0, 8, QOS_UNKNOWN, 0, 0, 124, 184,
	MSM8953_PNOC_SLV_1, MSM8953_PNOC_SLV_2, MSM8953_PNOC_SLV_0, MSM8953_PNOC_SLV_4, MSM8953_PNOC_SLV_6, MSM8953_PNOC_SLV_7, MSM8953_PNOC_SLV_8, MSM8953_PNOC_SLV_9, MSM8953_SLAVE_TCU, MSM8953_SLAVE_GRAPHICS_3D_CFG, MSM8953_PNOC_SLV_3);
QNODE(pcnoc_s_0, MSM8953_PNOC_SLV_0, 0, 0, 4, QOS_UNKNOWN, 0, 0, 89, 118,
	MSM8953_SLAVE_PDM, MSM8953_SLAVE_SPDM_WRAPPER);
QNODE(pcnoc_s_1, MSM8953_PNOC_SLV_1, 0, 0, 4, QOS_UNKNOWN, 0, 0, 90, 119,
	MSM8953_SLAVE_TCSR);
QNODE(pcnoc_s_2, MSM8953_PNOC_SLV_2, 0, 0, 4, QOS_UNKNOWN, 0, 0, 91, 120,
	MSM8953_SLAVE_SNOC_CFG);
QNODE(pcnoc_s_3, MSM8953_PNOC_SLV_3, 0, 0, 4, QOS_UNKNOWN, 0, 0, 92, 121,
	MSM8953_SLAVE_TLMM, MSM8953_SLAVE_PRNG, MSM8953_SLAVE_BLSP_1, MSM8953_SLAVE_BLSP_2, MSM8953_SLAVE_MESSAGE_RAM);
QNODE(pcnoc_s_4, MSM8953_PNOC_SLV_4, 0, 1, 4, QOS_UNKNOWN, 0, 0, 93, 122,
	MSM8953_SLAVE_CAMERA_CFG, MSM8953_SLAVE_DISPLAY_CFG, MSM8953_SLAVE_VENUS_CFG);
QNODE(pcnoc_s_6, MSM8953_PNOC_SLV_6, 0, 0, 4, QOS_UNKNOWN, 0, 0, 94, 123,
	MSM8953_SLAVE_CRYPTO_0_CFG, MSM8953_SLAVE_SDCC_2, MSM8953_SLAVE_SDCC_1);
QNODE(pcnoc_s_7, MSM8953_PNOC_SLV_7, 0, 0, 4, QOS_UNKNOWN, 0, 0, 95, 124,
	MSM8953_SLAVE_PMIC_ARB);
QNODE(pcnoc_s_8, MSM8953_PNOC_SLV_8, 0, 1, 4, QOS_UNKNOWN, 0, 0, 96, 125,
	MSM8953_SLAVE_USB3);
QNODE(pcnoc_s_9, MSM8953_PNOC_SLV_9, 0, 1, 4, QOS_UNKNOWN, 0, 0, 97, 126,
	MSM8953_SLAVE_IPA_CFG);
QNODE(slv_spdm, MSM8953_SLAVE_SPDM_WRAPPER, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 60, 0);
QNODE(slv_pdm, MSM8953_SLAVE_PDM, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 41, 0);
QNODE(slv_tcsr, MSM8953_SLAVE_TCSR, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 50, 0);
QNODE(slv_snoc_cfg, MSM8953_SLAVE_SNOC_CFG, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 70, 0);
QNODE(slv_tlmm, MSM8953_SLAVE_TLMM, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 51, 0);
QNODE(slv_message_ram, MSM8953_SLAVE_MESSAGE_RAM, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 55, 0);
QNODE(slv_blsp_1, MSM8953_SLAVE_BLSP_1, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 39, 0);
QNODE(slv_blsp_2, MSM8953_SLAVE_BLSP_2, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 37, 0);
QNODE(slv_prng, MSM8953_SLAVE_PRNG, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 44, 0);
QNODE(slv_camera_ss_cfg, MSM8953_SLAVE_CAMERA_CFG, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 3, 0);
QNODE(slv_disp_ss_cfg, MSM8953_SLAVE_DISPLAY_CFG, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 4, 0);
QNODE(slv_venus_cfg, MSM8953_SLAVE_VENUS_CFG, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 10, 0);
QNODE(slv_gpu_cfg, MSM8953_SLAVE_GRAPHICS_3D_CFG, 0, 1, 8, QOS_UNKNOWN, 0, 0, -1, 11, 0);
QNODE(slv_sdcc_1, MSM8953_SLAVE_SDCC_1, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 31, 0);
QNODE(slv_sdcc_2, MSM8953_SLAVE_SDCC_2, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 33, 0);
QNODE(slv_crypto_0_cfg, MSM8953_SLAVE_CRYPTO_0_CFG, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 52, 0);
QNODE(slv_pmic_arb, MSM8953_SLAVE_PMIC_ARB, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 59, 0);
QNODE(slv_usb3, MSM8953_SLAVE_USB3, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 22, 0);
QNODE(slv_ipa_cfg, MSM8953_SLAVE_IPA_CFG, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 183, 0);
QNODE(slv_tcu, MSM8953_SLAVE_TCU, 0, 1, 8, QOS_UNKNOWN, 0, 0, -1, 133, 0);
QNODE(slv_pcnoc_snoc, MSM8953_PNOC_SNOC_SLV, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 45,
	MSM8953_PNOC_SNOC_MAS);
QNODE(mas_qdss_bam, MSM8953_MASTER_QDSS_BAM, 11, 1, 4, QOS_FIXED, 1, 1, 19, -1,
	MSM8953_SNOC_QDSS_INT);
QNODE(mas_bimc_snoc, MSM8953_BIMC_SNOC_MAS, 0, 0, 8, QOS_UNKNOWN, 0, 0, 21, -1,
	MSM8953_SNOC_INT_0, MSM8953_SNOC_INT_1, MSM8953_SNOC_INT_2);
QNODE(mas_pcnoc_snoc, MSM8953_PNOC_SNOC_MAS, 5, 0, 8, QOS_FIXED, 0, 0, 29, -1,
	MSM8953_SNOC_INT_0, MSM8953_SNOC_INT_1, MSM8953_SNOC_BIMC_1_SLV);
QNODE(mas_ipa, MSM8953_MASTER_IPA, 14, 1, 8, QOS_FIXED, 0, 0, 59, -1,
	MSM8953_SNOC_INT_0, MSM8953_SNOC_INT_1, MSM8953_SNOC_BIMC_1_SLV);
QNODE(mas_qdss_etr, MSM8953_MASTER_QDSS_ETR, 10, 1, 8, QOS_FIXED, 1, 1, 31, -1,
	MSM8953_SNOC_QDSS_INT);
QNODE(qdss_int, MSM8953_SNOC_QDSS_INT, 0, 1, 8, QOS_UNKNOWN, 0, 0, 98, 128,
	MSM8953_SNOC_INT_1, MSM8953_SNOC_BIMC_1_SLV);
QNODE(snoc_int_0, MSM8953_SNOC_INT_0, 0, 1, 8, QOS_UNKNOWN, 0, 0, 99, 130,
	MSM8953_SLAVE_LPASS, MSM8953_SLAVE_WCSS, MSM8953_SLAVE_APPSS);
QNODE(snoc_int_1, MSM8953_SNOC_INT_1, 0, 0, 8, QOS_UNKNOWN, 0, 0, 100, 131,
	MSM8953_SLAVE_QDSS_STM, MSM8953_SLAVE_OCIMEM, MSM8953_SNOC_PNOC_SLV);
QNODE(snoc_int_2, MSM8953_SNOC_INT_2, 0, 1, 8, QOS_UNKNOWN, 0, 0, 134, 197,
	MSM8953_SLAVE_CATS_128, MSM8953_SLAVE_OCMEM_64);
QNODE(slv_kpss_ahb, MSM8953_SLAVE_APPSS, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 20, 0);
QNODE(slv_wcss, MSM8953_SLAVE_WCSS, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 23, 0);
QNODE(slv_snoc_bimc_1, MSM8953_SNOC_BIMC_1_SLV, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 104,
	MSM8953_SNOC_BIMC_1_MAS);
QNODE(slv_imem, MSM8953_SLAVE_OCIMEM, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 26, 0);
QNODE(slv_snoc_pcnoc, MSM8953_SNOC_PNOC_SLV, 0, 0, 8, QOS_UNKNOWN, 0, 0, -1, 28,
	MSM8953_SNOC_PNOC_MAS);
QNODE(slv_qdss_stm, MSM8953_SLAVE_QDSS_STM, 0, 0, 4, QOS_UNKNOWN, 0, 0, -1, 30, 0);
QNODE(slv_cats_1, MSM8953_SLAVE_OCMEM_64, 0, 1, 8, QOS_UNKNOWN, 0, 0, -1, 107, 0);
QNODE(slv_lpass, MSM8953_SLAVE_LPASS, 0, 1, 4, QOS_UNKNOWN, 0, 0, -1, 21, 0);
QNODE(mas_jpeg, MSM8953_MASTER_JPEG, 6, 1, 16, QOS_BYPASS, 0, 0, 7, -1,
	MSM8953_SNOC_BIMC_2_SLV);
QNODE(mas_mdp, MSM8953_MASTER_MDP_PORT0, 7, 1, 16, QOS_BYPASS, 0, 0, 8, -1,
	MSM8953_SNOC_BIMC_0_SLV);
QNODE(mas_venus, MSM8953_MASTER_VIDEO_P0, 8, 1, 16, QOS_BYPASS, 0, 0, 9, -1,
	MSM8953_SNOC_BIMC_2_SLV);
QNODE(mas_vfe0, MSM8953_MASTER_VFE, 9, 1, 16, QOS_BYPASS, 0, 0, 11, -1,
	MSM8953_SNOC_BIMC_0_SLV);
QNODE(mas_vfe1, MSM8953_MASTER_VFE1, 13, 1, 16, QOS_BYPASS, 0, 0, 133, -1,
	MSM8953_SNOC_BIMC_0_SLV);
QNODE(mas_cpp, MSM8953_MASTER_CPP, 12, 1, 16, QOS_BYPASS, 0, 0, 115, -1,
	MSM8953_SNOC_BIMC_2_SLV);
QNODE(slv_snoc_bimc_0, MSM8953_SNOC_BIMC_0_SLV, 0, 1, 16, QOS_UNKNOWN, 0, 0, -1, 24,
	MSM8953_SNOC_BIMC_0_MAS);
QNODE(slv_snoc_bimc_2, MSM8953_SNOC_BIMC_2_SLV, 0, 1, 16, QOS_UNKNOWN, 0, 0, -1, 137,
	MSM8953_SNOC_BIMC_2_MAS);
QNODE(slv_cats_0, MSM8953_SLAVE_CATS_128, 0, 1, 16, QOS_UNKNOWN, 0, 0, -1, 106, 0);

struct msm8953_icc_desc {
	struct msm8953_icc_node **nodes;
	size_t num_nodes;
	bool extra_clks;
	void (*node_qos_init)(struct msm8953_icc_node *, struct regmap*);
};

static struct msm8953_icc_node *msm8953_bimc_nodes[] = {
	[MAS_APPS_PROC] = &mas_apps_proc,
	[MAS_OXILI] = &mas_oxili,
	[MAS_SNOC_BIMC_0] = &mas_snoc_bimc_0,
	[MAS_SNOC_BIMC_2] = &mas_snoc_bimc_2,
	[MAS_SNOC_BIMC_1] = &mas_snoc_bimc_1,
	[MAS_TCU_0] = &mas_tcu_0,
	[SLV_EBI] = &slv_ebi,
	[SLV_BIMC_SNOC] = &slv_bimc_snoc,
};

static struct msm8953_icc_desc msm8953_bimc = {
	.nodes = msm8953_bimc_nodes,
	.num_nodes = ARRAY_SIZE(msm8953_bimc_nodes),
	.node_qos_init = msm8953_bimc_node_init,
};

static struct msm8953_icc_node *msm8953_pcnoc_nodes[] = {
	[MAS_SPDM] = &mas_spdm,
	[MAS_BLSP_1] = &mas_blsp_1,
	[MAS_BLSP_2] = &mas_blsp_2,
	[MAS_USB3] = &mas_usb3,
	[MAS_CRYPTO] = &mas_crypto,
	[MAS_SDCC_1] = &mas_sdcc_1,
	[MAS_SDCC_2] = &mas_sdcc_2,
	[MAS_SNOC_PCNOC] = &mas_snoc_pcnoc,
	[PCNOC_M_0] = &pcnoc_m_0,
	[PCNOC_M_1] = &pcnoc_m_1,
	[PCNOC_INT_1] = &pcnoc_int_1,
	[PCNOC_INT_2] = &pcnoc_int_2,
	[PCNOC_S_0] = &pcnoc_s_0,
	[PCNOC_S_1] = &pcnoc_s_1,
	[PCNOC_S_2] = &pcnoc_s_2,
	[PCNOC_S_3] = &pcnoc_s_3,
	[PCNOC_S_4] = &pcnoc_s_4,
	[PCNOC_S_6] = &pcnoc_s_6,
	[PCNOC_S_7] = &pcnoc_s_7,
	[PCNOC_S_8] = &pcnoc_s_8,
	[PCNOC_S_9] = &pcnoc_s_9,
	[SLV_SPDM] = &slv_spdm,
	[SLV_PDM] = &slv_pdm,
	[SLV_TCSR] = &slv_tcsr,
	[SLV_SNOC_CFG] = &slv_snoc_cfg,
	[SLV_TLMM] = &slv_tlmm,
	[SLV_MESSAGE_RAM] = &slv_message_ram,
	[SLV_BLSP_1] = &slv_blsp_1,
	[SLV_BLSP_2] = &slv_blsp_2,
	[SLV_PRNG] = &slv_prng,
	[SLV_CAMERA_SS_CFG] = &slv_camera_ss_cfg,
	[SLV_DISP_SS_CFG] = &slv_disp_ss_cfg,
	[SLV_VENUS_CFG] = &slv_venus_cfg,
	[SLV_GPU_CFG] = &slv_gpu_cfg,
	[SLV_SDCC_1] = &slv_sdcc_1,
	[SLV_SDCC_2] = &slv_sdcc_2,
	[SLV_CRYPTO_0_CFG] = &slv_crypto_0_cfg,
	[SLV_PMIC_ARB] = &slv_pmic_arb,
	[SLV_USB3] = &slv_usb3,
	[SLV_IPA_CFG] = &slv_ipa_cfg,
	[SLV_TCU] = &slv_tcu,
	[SLV_PCNOC_SNOC] = &slv_pcnoc_snoc,
};

static struct msm8953_icc_desc msm8953_pcnoc = {
	.nodes = msm8953_pcnoc_nodes,
	.num_nodes = ARRAY_SIZE(msm8953_pcnoc_nodes),
	.node_qos_init = msm8953_noc_node_init,
};

static struct msm8953_icc_node *msm8953_snoc_nodes[] = {
	[MAS_QDSS_BAM] = &mas_qdss_bam,
	[MAS_BIMC_SNOC] = &mas_bimc_snoc,
	[MAS_PCNOC_SNOC] = &mas_pcnoc_snoc,
	[MAS_IPA] = &mas_ipa,
	[MAS_QDSS_ETR] = &mas_qdss_etr,
	[QDSS_INT] = &qdss_int,
	[SNOC_INT_0] = &snoc_int_0,
	[SNOC_INT_1] = &snoc_int_1,
	[SNOC_INT_2] = &snoc_int_2,
	[SLV_KPSS_AHB] = &slv_kpss_ahb,
	[SLV_WCSS] = &slv_wcss,
	[SLV_SNOC_BIMC_1] = &slv_snoc_bimc_1,
	[SLV_IMEM] = &slv_imem,
	[SLV_SNOC_PCNOC] = &slv_snoc_pcnoc,
	[SLV_QDSS_STM] = &slv_qdss_stm,
	[SLV_CATS_1] = &slv_cats_1,
	[SLV_LPASS] = &slv_lpass,
	[MAS_JPEG] = &mas_jpeg,
	[MAS_MDP] = &mas_mdp,
	[MAS_VENUS] = &mas_venus,
	[MAS_VFE0] = &mas_vfe0,
	[MAS_VFE1] = &mas_vfe1,
	[MAS_CPP] = &mas_cpp,
	[SLV_SNOC_BIMC_0] = &slv_snoc_bimc_0,
	[SLV_SNOC_BIMC_2] = &slv_snoc_bimc_2,
	[SLV_CATS_0] = &slv_cats_0,
};

static struct msm8953_icc_desc msm8953_snoc = {
	.nodes = msm8953_snoc_nodes,
	.num_nodes = ARRAY_SIZE(msm8953_snoc_nodes),
	.node_qos_init = msm8953_noc_node_init,
	.extra_clks = true,
};

static void msm8953_bimc_node_init(struct msm8953_icc_node *qn, 
				  struct regmap* rmap)
{
	int health_lvl;
	u32 bke_en = 0;

	switch (qn->qos_mode) {
	case QOS_FIXED:
		for (health_lvl = 0; health_lvl < 4; health_lvl++) {
			regmap_update_bits(rmap, BIMC_BKE_HEALTH_REG(qn->qport, health_lvl), 
						 BIMC_BKE_HEALTH_PRIOLVL_MASK,
						 qn->prio0 << BIMC_BKE_HEALTH_PRIOLVL_SHIFT);

			regmap_update_bits(rmap, BIMC_BKE_HEALTH_REG(qn->qport, health_lvl), 
						 BIMC_BKE_HEALTH_AREQPRIO_MASK,
						 qn->prio1 << BIMC_BKE_HEALTH_AREQPRIO_SHIFT);

			if (health_lvl < 3)
				regmap_update_bits(rmap, 
						   BIMC_BKE_HEALTH_REG(qn->qport, health_lvl), 
						   BIMC_BKE_HEALTH_LIMIT_CMDS_MASK, 0);
		}
		bke_en = 1 << BIMC_BKE_ENA_SHIFT;
		break;
	case QOS_BYPASS:
		break;
	default:
		return;
	}

	regmap_update_bits(rmap, BIMC_BKE_ENA_REG(qn->qport), BIMC_BKE_ENA_MASK, bke_en);
}

static void msm8953_noc_node_init(struct msm8953_icc_node *qn, 
				  struct regmap* rmap)
{
	u32 mode = 0;

	switch (qn->qos_mode) {
	case QOS_BYPASS:
		mode = NOC_QOS_MODE_BYPASS;
		break;
	case QOS_FIXED:
		regmap_update_bits(rmap, 
				NOC_QOS_PRIO_REG(qn->qport),
				NOC_QOS_PRIO_P0_MASK,
				qn->prio0 & NOC_QOS_PRIO_P0_SHIFT);
		regmap_update_bits(rmap, 
				NOC_QOS_PRIO_REG(qn->qport),
				NOC_QOS_PRIO_P1_MASK, 
				qn->prio1 & NOC_QOS_PRIO_P0_SHIFT);
		mode = NOC_QOS_MODE_FIXED;
		break;
	default:
		return;
	}

	regmap_update_bits(rmap, 
			   NOC_QOS_MODE_REG(qn->qport),
			   NOC_QOS_MODE_MASK,
			   mode);
}

static int msm8953_icc_set(struct icc_node *src, struct icc_node *dst)
{
	struct msm8953_icc_provider *qp;
	struct msm8953_icc_node *qn;
	u64 sum_bw, max_peak_bw, rate;
	u32 agg_avg = 0, agg_peak = 0;
	struct icc_provider *provider;
	struct icc_node *n;
	int ret, i;

	qn = src->data;
	provider = src->provider;
	qp = to_msm8953_provider(provider);

	list_for_each_entry(n, &provider->nodes, node_list)
		provider->aggregate(n, 0, n->avg_bw, n->peak_bw,
				    &agg_avg, &agg_peak);

	sum_bw = icc_units_to_bps(agg_avg);
	max_peak_bw = icc_units_to_bps(agg_peak);

	/* send bandwidth request message to the RPM processor */
	if (!qn->ap_owned) {
		if (qn->mas_rpm_id) {
			ret = qcom_icc_rpm_smd_send(QCOM_SMD_RPM_ACTIVE_STATE,
					RPM_BUS_MASTER_REQ,
					qn->mas_rpm_id - 1,
					sum_bw);
			if (ret) {
				pr_err("qcom_icc_rpm_smd_send mas %d error %d\n",
						qn->mas_rpm_id - 1, ret);
				return ret;
			}
		}

		if (qn->slv_rpm_id) {
			ret = qcom_icc_rpm_smd_send(QCOM_SMD_RPM_ACTIVE_STATE,
					RPM_BUS_SLAVE_REQ,
					qn->slv_rpm_id - 1,
					sum_bw);
			if (ret) {
				pr_err("qcom_icc_rpm_smd_send slv error %d\n",
						ret);
				return ret;
			}
		}
	}

	rate = max(sum_bw, max_peak_bw);

	do_div(rate, qn->buswidth);

	if (qn->rate == rate)
		return 0;

	for (i = 2 * qn->clk_sel;
	     i < min(qp->num_clk_pairs * 2, qn->clk_sel * 2 + 2); i++) {
		ret = clk_set_rate(qp->bus_clks[i].clk, rate);
		if (ret) {
			pr_err("%s clk_set_rate error: %d\n",
			       qp->bus_clks[i].id, ret);
			return ret;
		}
	}

	qn->rate = rate;

	return 0;
}

static int msm8953_qnoc_probe(struct platform_device *pdev)
{
	const struct msm8953_icc_desc *desc;
	struct msm8953_icc_node **qnodes;
	struct msm8953_icc_provider *qp;
	struct device *dev = &pdev->dev;
	struct icc_onecell_data *data;
	struct icc_provider *provider;
	struct icc_node *node;
	size_t num_nodes, i;
	struct regmap *regmap;
	struct resource *res;
	void __iomem *base;
	struct regmap_config icc_regmap_cfg = {
		.reg_bits = 32,
		.reg_stride = 4,
		.val_bits = 32,
		.fast_io = true,
	};
	int ret;

	/* wait for the RPM proxy */
	if (!qcom_icc_rpm_smd_available())
		return -EPROBE_DEFER;

	desc = of_device_get_match_data(dev);
	if (!desc)
		return -EINVAL;

	if (desc->node_qos_init) {
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

		base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
		if (IS_ERR(base))
			return PTR_ERR(base);

		icc_regmap_cfg.max_register = resource_size(res) - 4;

		regmap = devm_regmap_init_mmio(&pdev->dev, base, &icc_regmap_cfg);
		if (IS_ERR(regmap))
			return PTR_ERR(regmap);
	}

	qnodes = desc->nodes;
	num_nodes = desc->num_nodes;

	qp = devm_kzalloc(dev, sizeof(*qp), GFP_KERNEL);
	if (!qp)
		return -ENOMEM;

	data = devm_kzalloc(dev, struct_size(data, nodes, num_nodes),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	qp->num_clk_pairs = ARRAY_SIZE(msm8953_bus_clocks) / 2;
	if (!desc->extra_clks)
		qp->num_clk_pairs --;

	qp->bus_clks = devm_kmemdup(dev, msm8953_bus_clocks,
				    2 * sizeof(msm8953_bus_clocks[0]) * qp->num_clk_pairs,
				    GFP_KERNEL);
	if (!qp->bus_clks)
		return -ENOMEM;

	ret = devm_clk_bulk_get(dev, qp->num_clk_pairs * 2, qp->bus_clks);
	if (ret)
		return ret;

	ret = clk_bulk_prepare_enable(qp->num_clk_pairs * 2, qp->bus_clks);
	if (ret)
		return ret;

	provider = &qp->provider;
	INIT_LIST_HEAD(&provider->nodes);
	provider->dev = dev;
	provider->set = msm8953_icc_set;
	provider->aggregate = icc_std_aggregate;
	provider->xlate = of_icc_xlate_onecell;
	provider->data = data;

	ret = icc_provider_add(provider);
	if (ret) {
		dev_err(dev, "error adding interconnect provider: %d\n", ret);
		clk_bulk_disable_unprepare(qp->num_clk_pairs * 2, qp->bus_clks);
		return ret;
	}

	for (i = 0; i < num_nodes; i++) {
		size_t j;
		
		node = icc_node_create(qnodes[i]->id);
		if (IS_ERR(node)) {
			ret = PTR_ERR(node);
			goto err;
		}

		node->name = qnodes[i]->name;
		node->data = qnodes[i];
		icc_node_add(node, provider);

		for (j = 0; j < qnodes[i]->num_links; j++)
			icc_link_create(node, qnodes[i]->links[j]);

		data->nodes[i] = node;
	}

	data->num_nodes = num_nodes;

	for (i = 0; i < num_nodes; i++) {
		if (desc->node_qos_init && qnodes[i]->ap_owned)
			desc->node_qos_init(qnodes[i], regmap);
	}

	platform_set_drvdata(pdev, qp);

	return 0;

err:
	icc_nodes_remove(provider);
	icc_provider_del(provider);
	clk_bulk_disable_unprepare(qp->num_clk_pairs * 2, qp->bus_clks);

	return ret;
}

static int msm8953_qnoc_remove(struct platform_device *pdev)
{
	struct msm8953_icc_provider *qp = platform_get_drvdata(pdev);

	icc_nodes_remove(&qp->provider);
	clk_bulk_disable_unprepare(qp->num_clk_pairs * 2, qp->bus_clks);
	return icc_provider_del(&qp->provider);
}

static const struct of_device_id msm8953_noc_of_match[] = {
	{ .compatible = "qcom,msm8953-bimc", .data = &msm8953_bimc },
	{ .compatible = "qcom,msm8953-pcnoc", .data = &msm8953_pcnoc },
	{ .compatible = "qcom,msm8953-snoc", .data = &msm8953_snoc },
	{ }
};
MODULE_DEVICE_TABLE(of, msm8953_noc_of_match);

static struct platform_driver msm8953_noc_driver = {
	.probe = msm8953_qnoc_probe,
	.remove = msm8953_qnoc_remove,
	.driver = {
		.name = "qnoc-msm8953",
		.of_match_table = msm8953_noc_of_match,
	},
};
module_platform_driver(msm8953_noc_driver);
MODULE_DESCRIPTION("Qualcomm MSM8953 NoC driver");
MODULE_LICENSE("GPL v2");
