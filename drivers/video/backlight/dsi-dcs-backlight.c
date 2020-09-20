// SPDX-License-Identifier: GPL-2.0-only
/*
 * dsi-dcs-backlight.c - DSI-controlled backlight
 */

#include <linux/backlight.h>
#include <linux/fb.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include <drm/drm_mipi_dsi.h>
#include <video/mipi_display.h>

#include <linux/platform_data/dsi_backlight.h>

static int mipi_dsi_generic_set_brightness(struct backlight_device *bl)
{
	struct dsi_backlight_platform_data *pdata = bl_get_data(bl);
	u8 payload;
	int ret = -EAGAIN;

	int brightness = bl->props.brightness;
	if (bl->props.power != FB_BLANK_UNBLANK ||
	    bl->props.fb_blank != FB_BLANK_UNBLANK ||
	    bl->props.state & (BL_CORE_SUSPENDED | BL_CORE_FBBLANK))
		brightness = 0;

	payload = (u8) clamp(brightness, 0, 255);

	if (pdata->prepared)
		ret = mipi_dsi_dcs_write(pdata->dsi,
					 MIPI_DCS_SET_DISPLAY_BRIGHTNESS,
					 &payload, sizeof(payload));

	return ret < 0 ? ret : 0;
}

static int mipi_dsi_generic_get_brightness(struct backlight_device *bl)
{
	struct dsi_backlight_platform_data *pdata = bl_get_data(bl);
	u8 brightness;
	int ret = -EAGAIN;

	if (pdata->prepared)
		ret = mipi_dsi_dcs_read(pdata->dsi,
					MIPI_DCS_GET_DISPLAY_BRIGHTNESS,
					&brightness, sizeof(brightness));

	if (ret == 0)
		ret = -ENODATA;
	else
		ret = bl->props.brightness = brightness;

	return ret;
}

static const struct backlight_ops dsi_dcs_backlight_ops = {
	.update_status = mipi_dsi_generic_set_brightness,
	.get_brightness = mipi_dsi_generic_get_brightness,
};

static int dsi_dcs_backlight_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dsi_backlight_platform_data *pdata = dev_get_drvdata(dev->parent);
	struct backlight_properties props = { 0 };
	u32 max_level = 200;

	if (!pdata)
		return -ENODEV;

	of_property_read_u32(dev->of_node, "max-level", &max_level);

	props.max_brightness = min(max_level, 200U);
	props.brightness = props.max_brightness;
	props.brightness -= props.max_brightness / 5;
	props.type = BACKLIGHT_RAW;

	platform_set_drvdata(pdev, pdata);

	pdata->backlight = devm_backlight_device_register(dev, dev_name(dev), dev, pdata,
					    &dsi_dcs_backlight_ops, &props);
	if (IS_ERR(pdata->backlight)) {
		dev_err(dev, "Failed to register backlight\n");
		return PTR_ERR(pdata->backlight);
	}

	return 0;
}

static struct of_device_id dsi_dcs_backlight_of_match[] = {
	{ .compatible = "mipi-dcs-backlight" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, dsi_dcs_backlight_of_match);

static struct platform_driver dsi_dcs_backlight_driver = {
	.driver = {
		.name		= "mipi-dcs-backlight",
		.of_match_table = dsi_dcs_backlight_of_match,
	},
	.probe = dsi_dcs_backlight_probe,
};

module_platform_driver(dsi_dcs_backlight_driver);

MODULE_DESCRIPTION("DCS-controlled backlight driver");
MODULE_LICENSE("GPL");
