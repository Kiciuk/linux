// SPDX-License-Identifier: GPL-2.0

#ifndef __SOC_MEDIATEK_MT6789_PM_DOMAINS_H
#define __SOC_MEDIATEK_MT6789_PM_DOMAINS_H

#include "mtk-pm-domains.h"
#include <dt-bindings/power/mt6789-power.h>

/*
 * MT6789 power domain support
 */

static const struct scpsys_domain_data scpsys_domain_data_mt6789[] = {
	[MT6789_POWER_DOMAIN_MD] = {
		.name = "md",
		.sta_mask = BIT(0),
		.ctl_offs = 0x300,
		.ext_buck_iso_offs = 0x398,
		.ext_buck_iso_mask = 0x3,
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, 1<<7, 0x02A0, 0x02A4, 0x0228),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_INFRA_VDNR_MD, 0x0B84, 0x0B88, 0x0B90),
		},
		//.caps = MTK_SCPD_MD_OPS | MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_CONN] = {
		.name = "conn",
		.sta_mask = BIT(1),
		.ctl_offs = 0x304,
		.bp_cfg = {
			
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_CONN, 0x02A0, 0x02A4, 0x0228),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_CONN_2ND, 0x02A0, 0x02A4, 0x0228),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_1_CONN, 0x02A8, 0x02AC, 0x0258),
	
		},
		//.caps = MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_MFG0] = {
		.name = "mfg0",
		.sta_mask = BIT(2),
		.ctl_offs = 0x308,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.caps = MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_MFG1] = {
		.name = "mfg1",
		.sta_mask = BIT(3),
		.ctl_offs = 0x30C,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	//	.basic_clk_name = {"mfg"},
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_1_MFG1, 0x02A8, 0x02AC, 0x0258),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_2_MFG1, 0x0714, 0x0718, 0x0724),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MFG1, 0x02A0, 0x02A4, 0x0228),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_2_MFG1_2ND, 0x0714, 0x0718, 0x0724),
		},
		//.caps = MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_MFG2] = {
		.name = "mfg2",
		.sta_mask = BIT(4),
		.ctl_offs = 0x310,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.caps = MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_MFG3] = {
		.name = "mfg3",
		.sta_mask = BIT(5),
		.ctl_offs = 0x314,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.caps = MTK_SCPD_BYPASS_INIT_ON,
	},
	[MT6789_POWER_DOMAIN_ISP] = {
		.name = "isp",
		.sta_mask = BIT(13),
		.ctl_offs = 0x334,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.basic_clk_name = {"isp"},
		// .subsys_clk_prefix = "isp",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_2_ISP, 0x0DCC, 0x0DD0, 0x0DD8),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_2_ISP_2ND, 0x0DCC, 0x0DD0, 0x0DD8),

		},
	},
	[MT6789_POWER_DOMAIN_IPE] = {
		.name = "ipe",
		.sta_mask = BIT(15),
		.ctl_offs = 0x33C,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.basic_clk_name = {"ipe"},
		// .subsys_clk_prefix = "ipe",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_IPE, 0x02D4, 0x02D8, 0x02EC),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_IPE_2ND, 0x02D4, 0x02D8, 0x02EC),
		},
	},
	[MT6789_POWER_DOMAIN_VDEC] = {
		.name = "vdec",
		.sta_mask = BIT(16),
		.ctl_offs = 0x340,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.basic_clk_name = {"vdec"},
		// .subsys_clk_prefix = "vdec",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_VDEC, 0x02D4, 0x02D8, 0x02EC),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_VDEC_2ND, 0x02D4, 0x02D8, 0x02EC),

		},
	},
	[MT6789_POWER_DOMAIN_VENC] = {
		.name = "venc",
		.sta_mask = BIT(18),
		.ctl_offs = 0x348,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		//.basic_clk_name = {"venc"},
		// .subsys_clk_prefix = "venc",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_VENC, 0x02D4, 0x02D8, 0x02EC),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_VENC_2ND, 0x02D4, 0x02D8, 0x02EC),

		},
	},
	[MT6789_POWER_DOMAIN_DISP] = {
		.name = "disp",
		.sta_mask = BIT(21),
		.ctl_offs = 0x354,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	//	.basic_clk_name = {"disp", "mdp"},
		// .subsys_clk_prefix = "disp",
		//.subsys_lp_clk_prefix = "disp_lp",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_DISP, 0x02D4, 0x02D8, 0x02EC),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_DISP, 0x02A0, 0x02A4, 0x0228),

		},
	},
	[MT6789_POWER_DOMAIN_AUDIO] = {
		.name = "audio",
		.sta_mask = BIT(22),
		.ctl_offs = 0x358,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	//	.basic_clk_name = {"audio"},
		// .subsys_clk_prefix = "audio",
		.bp_cfg = {
				BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_2_AUDIO, 0x0714, 0x0718, 0x0724),
		},
	},
	[MT6789_POWER_DOMAIN_CAM] = {
		.name = "cam",
		.sta_mask = BIT(23),
		.ctl_offs = 0x35C,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	//	.basic_clk_name = {"cam"},
		// .subsys_clk_prefix = "cam",
		.bp_cfg = {
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_CAM, 0x02D4, 0x02D8, 0x02EC),
			BUS_PROT_WR_IGN(INFRA, MT6789_TOP_AXI_PROT_EN_MM_CAM_2ND, 0x02D4, 0x02D8, 0x02EC),
		},
	},
	[MT6789_POWER_DOMAIN_CAM_RAWA] = {
		.name = "cam_rawa",
		.sta_mask = BIT(24),
		.ctl_offs = 0x360,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		// .subsys_clk_prefix = "cam_rawa",
	},
	[MT6789_POWER_DOMAIN_CAM_RAWB] = {
		.name = "cam_rawb",
		.sta_mask = BIT(25),
		.ctl_offs = 0x364,
		.pwr_sta_offs = 0x016c,
		.pwr_sta2nd_offs = 0x0170,
 		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		// .subsys_clk_prefix = "cam_rawb",
	},
};


static const struct scpsys_soc_data mt6789_scpsys_data = {
	.domains_data = scpsys_domain_data_mt6789,
	.num_domains = ARRAY_SIZE(scpsys_domain_data_mt6789),
};


#endif /* __SOC_MEDIATEK_MT6789_PM_DOMAINS_H */
