/* SPDX-License-Identifier: GPL-2.0-only
 * Copyright (c) 2020, The Linux Foundation. All rights reserved. */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/display_timing.h>
#include <video/videomode.h>
#include <video/of_display_timing.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

#include <dt-bindings/display/mipi-dsi-generic.h>

#include <linux/platform_data/dsi_backlight.h>

#undef MIPI_CMD
#define MIPI_CMD(code, len, payload)	code
#define to_mipi_dsi_generic(drm_panel) \
	container_of(drm_panel, struct mipi_dsi_generic, panel)

struct mipi_dsi_cmds {
	int size;
	uint8_t data[];
};

struct mipi_dsi_generic {
	struct drm_panel panel;
	struct dsi_backlight_platform_data pdata;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	int num_supplies;
	struct gpio_desc *reset_gpio;
	struct mipi_dsi_cmds *on_cmds;
	struct mipi_dsi_cmds *off_cmds;
	unsigned long on_mode_mask;
	unsigned long off_mode_mask;
};

static struct mipi_dsi_cmds *mipi_dsi_generic_parse_cmds(struct device *dev, const char *prop)
{
	struct device_node* np = dev->of_node;
	struct mipi_dsi_cmds *cmds;
	int n_elems, ret;

	n_elems = of_property_count_u8_elems(np, prop);
	if (n_elems <= 0) {
		dev_dbg(dev, "Could not parse \"%s\": %d\n", prop, n_elems);
		return NULL;
	}

	cmds = devm_kzalloc(dev, sizeof(*cmds) + n_elems, GFP_KERNEL);
	if (IS_ERR_OR_NULL(cmds))
		return cmds;

	cmds->size = n_elems;

	ret = of_property_read_u8_array(np, prop, &cmds->data[0], n_elems);
	if (ret)
		return ERR_PTR(ret < 0 ? ret : -EINVAL);

	return cmds;
}

static int mipi_dsi_generic_write_cmds(struct mipi_dsi_device *dsi,
				       struct mipi_dsi_cmds *cmds)
{
	struct mipi_dsi_generic *ctx = container_of(mipi_dsi_get_drvdata(dsi),
						    struct mipi_dsi_generic, pdata);
	int ret, bytes_left = cmds->size;
	uint8_t* buf = &cmds->data[0];
	int n;

	if (!cmds)
		return 0;

	for (n = 0; bytes_left >= 4; n ++) {
		int payload_len, code;
		uint8_t *payload;

		if (buf[0] != MIPI_CMD_BEGIN_MARKER)
			goto err_invalid;

		code = buf[1];
		payload_len = buf[2];
		payload = &buf[3];

		if ((4 + payload_len) > bytes_left ||
		     buf[3 + payload_len] != MIPI_CMD_END_MARKER)
			goto err_invalid;

		switch (code) {
		case MIPI_CMD_DELAY_MS():
			if (payload_len != 1)
				goto err_invalid;

			if (*payload > 20)
				msleep(*payload);
			else
				usleep_range(*payload * 1000, (*payload + 1) * 1000);
			break;
		case MIPI_CMD_GENERIC_WRITE(,):
			if (!payload_len)
				goto err_invalid;
			ret = mipi_dsi_generic_write(dsi, payload, payload_len);
			if (ret < 0)
				return ret;
			break;
		case MIPI_CMD_DCS_WRITE(,):
			if (!payload_len)
				goto err_invalid;
			ret = mipi_dsi_dcs_write_buffer(dsi, payload, payload_len);
			if (ret < 0)
				return ret;
			break;
		case MIPI_CMD_TURN_ON_PERIPHERAL:
			ret = mipi_dsi_turn_on_peripheral(dsi);
			if (ret < 0)
				return ret;
			break;
		case MIPI_CMD_BACKLIGHT_INIT:
			if (ctx->pdata.backlight_init) {
				ctx->pdata.backlight_init(&ctx->pdata);
				if (ret < 0)
					return ret;
			}
			break;
		case MIPI_CMD_SHUTDOWN_PERIPHERAL:
			ret = mipi_dsi_shutdown_peripheral(dsi);
			if (ret < 0)
				return ret;
			break;
		default:
			dev_err(&dsi->dev, "Unsupported helper command %d\n", buf[2]);
			return -EINVAL;
		}

		buf += 4 + payload_len;
		bytes_left -= 4 + payload_len;
	}

	if (!bytes_left)
		return 0;

err_invalid:
	dev_err(&dsi->dev, "invalid command at index %d\n", n);
	return -EINVAL;
}

static void mipi_dsi_generic_reset(struct mipi_dsi_generic *ctx)
{
	struct device* dev = &ctx->dsi->dev;
	struct device_node* np = dev->of_node;
	const char *prop = "reset-sequence";
	int seq_len, i, ret;

	if (!ctx->reset_gpio)
		return;

	seq_len = of_property_count_u32_elems(np, prop);
	if (seq_len < 0) {
		dev_dbg(dev, "Failed to get \"%s\": %d\n", prop, seq_len);
		return;
	}

	for (i = 0; i < seq_len; i ++)  {
		u32 item;
		ret = of_property_read_u32_index(np, prop, i, &item);

		if (i % 2) {
			if (item > 20)
				msleep(item);
			else
				usleep_range(item * 1000, (item + 1) * 1000);
		} else {
			gpiod_set_value_cansleep(ctx->reset_gpio, !!item);
		}
	}
}

static int mipi_dsi_generic_prepare(struct drm_panel *panel)
{
	struct mipi_dsi_generic *ctx = to_mipi_dsi_generic(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->pdata.prepared)
		return 0;

	ctx->pdata.prepared = true;

	pinctrl_pm_select_default_state(dev);

	ret = regulator_bulk_enable(ctx->num_supplies, ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	mipi_dsi_generic_reset(ctx);

	ctx->dsi->mode_flags &= ~ctx->off_mode_mask;
	ctx->dsi->mode_flags |= ctx->on_mode_mask;

	ret = mipi_dsi_generic_write_cmds(ctx->dsi, ctx->on_cmds);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ctx->num_supplies, ctx->supplies);
		return ret;
	}

	return 0;
}

static int mipi_dsi_generic_unprepare(struct drm_panel *panel)
{
	struct mipi_dsi_generic *ctx = to_mipi_dsi_generic(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->pdata.prepared)
		return 0;

	ctx->pdata.prepared = false;

	ctx->dsi->mode_flags &= ~ctx->on_mode_mask;
	ctx->dsi->mode_flags |= ctx->off_mode_mask;

	ret = mipi_dsi_generic_write_cmds(ctx->dsi, ctx->off_cmds);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);


	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ctx->num_supplies, ctx->supplies);
	pinctrl_pm_select_sleep_state(dev);

	return 0;
}

static const unsigned int mipi_dsi_generic_mode_map[][2] = {
	{ MIPI_GENERIC_DSI_MODE_VIDEO,		MIPI_DSI_MODE_VIDEO },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_BURST,	MIPI_DSI_MODE_VIDEO_BURST },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_SYNC_PULSE, MIPI_DSI_MODE_VIDEO_SYNC_PULSE },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_AUTO_VERT, MIPI_DSI_MODE_VIDEO_AUTO_VERT },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_HSE,	MIPI_DSI_MODE_VIDEO_HSE },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_HFP,	MIPI_DSI_MODE_VIDEO_HFP },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_HBP,	MIPI_DSI_MODE_VIDEO_HBP },
	{ MIPI_GENERIC_DSI_MODE_VIDEO_HSA,	MIPI_DSI_MODE_VIDEO_HSA },
	{ MIPI_GENERIC_DSI_MODE_VSYNC_FLUSH,	MIPI_DSI_MODE_VSYNC_FLUSH },
	{ MIPI_GENERIC_DSI_MODE_EOT_PACKET,	MIPI_DSI_MODE_EOT_PACKET },
	{ MIPI_GENERIC_DSI_CLOCK_NON_CONTINUOUS, MIPI_DSI_CLOCK_NON_CONTINUOUS },
	{ MIPI_GENERIC_DSI_MODE_LPM,		MIPI_DSI_MODE_LPM },
};

static int mipi_dsi_generic_of_get_format(struct mipi_dsi_generic *ctx)
{
	u32 format;
	int ret;
	struct mipi_dsi_device *dsi = ctx->dsi;

	ret = of_property_read_u32(ctx->dsi->dev.of_node, "dsi-format", &format);
	if (ret)
		return ret;

	switch (format) {
		case MIPI_GENERIC_DSI_FMT_RGB888:
			dsi->format = MIPI_DSI_FMT_RGB888;
			break;
		case MIPI_GENERIC_DSI_FMT_RGB666:
			dsi->format = MIPI_DSI_FMT_RGB666;
			break;
		case MIPI_GENERIC_DSI_FMT_RGB666_PACKED:
			dsi->format = MIPI_DSI_FMT_RGB666_PACKED;
			break;
		case MIPI_GENERIC_DSI_FMT_RGB565:
			dsi->format = MIPI_DSI_FMT_RGB565;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static int mipi_dsi_generic_of_get_mode(struct mipi_dsi_generic *ctx)
{
	u32 mode;
	int ret, i;

	ret = of_property_read_u32(ctx->dsi->dev.of_node, "dsi-mode", &mode);
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(mipi_dsi_generic_mode_map); i++)
		if (mode & mipi_dsi_generic_mode_map[i][0])
			ctx->dsi->mode_flags |= mipi_dsi_generic_mode_map[i][1];

	ctx->dsi->mode_flags = mode;
	return 0;
}

static int mipi_dsi_generic_get_modes(struct drm_panel *panel,
				struct drm_connector *connector)
{
	struct mipi_dsi_generic *ctx = to_mipi_dsi_generic(panel);
	struct device *dev = &ctx->dsi->dev;
	struct drm_display_mode *mode;
	struct display_timing timing;
	struct videomode vidmode;
	uint32_t width, height, rate;
	int ret;
	ret = of_get_display_timing(dev->of_node, "panel-timing", &timing);

	if (ret < 0) {
		dev_err(dev, "Failed to parse display timing:%d\n", ret);
		return ret;
	}

	ret = of_property_read_u32(dev->of_node, "panel-refresh-rate", &rate);
	ret = ret ?: of_property_read_u32(dev->of_node, "panel-width-mm", &width);
	ret = ret ?: of_property_read_u32(dev->of_node, "panel-height-mm", &height);
	if (ret)
		return ret;

	mode = drm_mode_create(connector->dev);
	if (!mode)
		return -ENOMEM;

	videomode_from_timing(&timing, &vidmode);
	drm_display_mode_from_videomode(&vidmode, mode);
	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	mode->vrefresh = rate;
	connector->display_info.width_mm = mode->width_mm = width;
	connector->display_info.height_mm = mode->height_mm = height;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs mipi_dsi_generic_panel_funcs = {
	.prepare = mipi_dsi_generic_prepare,
	.unprepare = mipi_dsi_generic_unprepare,
	.get_modes = mipi_dsi_generic_get_modes,
};

static int mipi_dsi_generic_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct mipi_dsi_generic *ctx;
	int ret, i;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->pdata.dsi = ctx->dsi = dsi;
	ctx->pdata.prepared = false;

	/* For DCS backlight */
	dev_set_drvdata(dev, &ctx->pdata);
	ret = devm_of_platform_populate(dev);
	if (ret < 0)
		return ret;

	ctx->num_supplies = of_property_count_strings(dev->of_node, "supply-names");

	if (ctx->num_supplies < 0)
		ctx->num_supplies = 0;

	if (ctx->num_supplies) {
		ctx->supplies = devm_kzalloc(dev, sizeof(ctx->supplies[0]) * ctx->num_supplies,
					     GFP_KERNEL);
		if (IS_ERR_OR_NULL(ctx->supplies))
			return -ENOMEM;

		for (i = 0; i < ctx->num_supplies; i++) {
			ret = of_property_read_string_index(dev->of_node, "supply-names",
								i, &ctx->supplies[i].supply);
			if (ret < 0)
				return ret;
		}

		ret = devm_regulator_bulk_get(dev, ctx->num_supplies, ctx->supplies);
		if (ret < 0) {
			dev_err(dev, "Failed to get regulators: %d\n", ret);
			return ret;
		}
	}

	ctx->on_cmds = mipi_dsi_generic_parse_cmds(dev, "dsi-on-commands");
	ctx->off_cmds = mipi_dsi_generic_parse_cmds(dev, "dsi-off-commands");

	if (of_property_read_bool(dev->of_node, "dsi-on-lp-mode"))
		ctx->on_mode_mask = MIPI_DSI_MODE_LPM;
	else if (of_property_read_bool(dev->of_node, "dsi-off-lp-mode"))
		ctx->off_mode_mask = MIPI_DSI_MODE_LPM;


	ret = mipi_dsi_generic_of_get_format(ctx);
	if (ret)
		return ret;

	ret = mipi_dsi_generic_of_get_mode(ctx);
	if (ret)
		return ret;

	ret = of_property_read_u32(dev->of_node, "dsi-lanes", &dsi->lanes);
	if (ret)
		return ret;

	ctx->reset_gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio)) {
		ret = PTR_ERR(ctx->reset_gpio);
		dev_err(dev, "Failed to get reset-gpios: %d\n", ret);
		return ret;
	}

	drm_panel_init(&ctx->panel, dev, &mipi_dsi_generic_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret) {
		dev_err(dev, "Failed to get backlight: %d\n", ret);
		return ret;
	}

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0) {
		dev_err(dev, "Failed to add panel: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}


	return 0;
}

static int mipi_dsi_generic_remove(struct mipi_dsi_device *dsi)
{
	struct mipi_dsi_generic *ctx = container_of(mipi_dsi_get_drvdata(dsi),
						    struct mipi_dsi_generic, pdata);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id mipi_dsi_generic_of_match[] = {
	{ .compatible = "panel-mipi-dsi-generic" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mipi_dsi_generic_of_match);

static struct mipi_dsi_driver mipi_dsi_generic_driver = {
	.probe = mipi_dsi_generic_probe,
	.remove = mipi_dsi_generic_remove,
	.driver = {
		.name = "mipi-dsi-generic-panel",
		.of_match_table = mipi_dsi_generic_of_match,
	},
};
module_mipi_dsi_driver(mipi_dsi_generic_driver);

MODULE_AUTHOR("Junak <junak.pub@gmail.com>");
MODULE_DESCRIPTION("Generic DRM driver for mipi dsi panels");
MODULE_LICENSE("GPL v2");
