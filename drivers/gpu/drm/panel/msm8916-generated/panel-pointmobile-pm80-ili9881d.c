// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct ili9881d {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
};

static inline struct ili9881d *to_ili9881d(struct drm_panel *panel)
{
	return container_of(panel, struct ili9881d, panel);
}

static void ili9881d_reset(struct ili9881d *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int ili9881d_on(struct ili9881d *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x98, 0x81, 0x03);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x01, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x02, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x03, 0x56);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x04, 0x13);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x05, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x06, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x07, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x09, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0a, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0b, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0c, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0d, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0e, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0f, 0x1b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x10, 0x1b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x12, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x13, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x14, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x15, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x16, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x17, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x18, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x19, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1a, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1b, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1c, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1d, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1e, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1f, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x20, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x21, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x22, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x23, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x24, 0x87);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x25, 0x86);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_GAMMA_CURVE, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x27, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x28, 0x3b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x29, 0x03);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x2a, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x2b, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_MEMORY_START,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_LUT, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_MEMORY_START,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x2f, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_PARTIAL_ROWS, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_PARTIAL_COLUMNS,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x32, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x33, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x34, 0x00);
	mipi_dsi_dcs_set_tear_on_multi(&dsi_ctx, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_ADDRESS_MODE, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x37, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x38, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x39, 0x35);
	mipi_dsi_dcs_set_pixel_format_multi(&dsi_ctx, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x3b, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_MEMORY_CONTINUE,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_3D_CONTROL, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_MEMORY_CONTINUE,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x3f, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_VSYNC_TIMING, 0x35);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x41, 0x88);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x42, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x43, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x44, 0x1f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x50, 0x01);
	mipi_dsi_dcs_set_display_brightness_multi(&dsi_ctx, 0x0023);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x52, 0x45);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x67);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x54, 0x89);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0xab);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x56, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x57, 0x23);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x58, 0x45);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x59, 0x67);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5a, 0x89);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5b, 0xab);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5c, 0xcd);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5d, 0xef);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_CABC_MIN_BRIGHTNESS,
				     0x13);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5f, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x60, 0x0c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x61, 0x0d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x62, 0x0e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x63, 0x0f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x64, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x65, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x66, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x67, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x68, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x69, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6a, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6b, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6c, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6d, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6e, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6f, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x70, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x71, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x72, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x73, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x74, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x75, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x76, 0x0c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x77, 0x0d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x78, 0x03);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x79, 0x0f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7a, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7b, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7c, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7d, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7e, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x7f, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x80, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x81, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x82, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x84, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x86, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x87, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x88, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x89, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x8a, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x98, 0x81, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x68, 0xdb);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6d, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x70, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x71, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x66, 0xfe);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6f, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x82, 0x1b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x84, 0x1b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x32, 0xac);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x8c, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_MEMORY_CONTINUE,
				     0xf5);
	mipi_dsi_dcs_set_pixel_format_multi(&dsi_ctx, 0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb5, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_PARTIAL_COLUMNS,
				     0x25);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x88, 0x23);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x98, 0x81, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x22, 0x0a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_PARTIAL_COLUMNS,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x50, 0x73);
	mipi_dsi_dcs_set_display_brightness_multi(&dsi_ctx, 0x0073);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x78);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0x78);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x60, 0x1c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x61, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x62, 0x0d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x63, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0, 0x1d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_DDB_START, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_PPS_START, 0x27);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa3, 0x12);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa4, 0x16);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa5, 0x2a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa6, 0x1e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa7, 0x1e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_DDB_CONTINUE,
				     0x99);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_PPS_CONTINUE,
				     0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xaa, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xab, 0x87);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xac, 0x19);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xad, 0x17);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xae, 0x4b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xaf, 0x1e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x26);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb1, 0x51);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2, 0x63);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x39);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x03);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1, 0x35);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc2, 0x44);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc3, 0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x1a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x2b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc6, 0x1d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc7, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc8, 0xae);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc9, 0x23);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xca, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb, 0xa4);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc, 0x1f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcd, 0x1e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce, 0x54);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcf, 0x28);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd0, 0x2b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd1, 0x66);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd2, 0x70);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd3, 0x39);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x98, 0x81, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x35);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_usleep_range(&dsi_ctx, 10000, 11000);

	return dsi_ctx.accum_err;
}

static int ili9881d_off(struct ili9881d *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int ili9881d_prepare(struct drm_panel *panel)
{
	struct ili9881d *ctx = to_ili9881d(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	ili9881d_reset(ctx);

	ret = ili9881d_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	return 0;
}

static int ili9881d_unprepare(struct drm_panel *panel)
{
	struct ili9881d *ctx = to_ili9881d(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = ili9881d_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	return 0;
}

static const struct drm_display_mode ili9881d_mode = {
	.clock = (720 + 78 + 16 + 100) * (1280 + 10 + 4 + 6) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 78,
	.hsync_end = 720 + 78 + 16,
	.htotal = 720 + 78 + 16 + 100,
	.vdisplay = 1280,
	.vsync_start = 1280 + 10,
	.vsync_end = 1280 + 10 + 4,
	.vtotal = 1280 + 10 + 4 + 6,
	.width_mm = 62,
	.height_mm = 110,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int ili9881d_get_modes(struct drm_panel *panel,
			      struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &ili9881d_mode);
}

static const struct drm_panel_funcs ili9881d_panel_funcs = {
	.prepare = ili9881d_prepare,
	.unprepare = ili9881d_unprepare,
	.get_modes = ili9881d_get_modes,
};

static int ili9881d_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ili9881d *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(ctx->supply))
		return dev_err_probe(dev, PTR_ERR(ctx->supply),
				     "Failed to get power regulator\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &ili9881d_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void ili9881d_remove(struct mipi_dsi_device *dsi)
{
	struct ili9881d *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ili9881d_of_match[] = {
	{ .compatible = "mdss,ili9881d" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili9881d_of_match);

static struct mipi_dsi_driver ili9881d_driver = {
	.probe = ili9881d_probe,
	.remove = ili9881d_remove,
	.driver = {
		.name = "panel-ili9881d",
		.of_match_table = ili9881d_of_match,
	},
};
module_mipi_dsi_driver(ili9881d_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ili9881d 720p video mode dsi panel");
MODULE_LICENSE("GPL");
