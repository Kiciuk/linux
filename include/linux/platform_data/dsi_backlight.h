/* SPDX-License-Identifier: GPL-2.0-only
 * Copyright (c) 2020, The Linux Foundation. All rights reserved. */

#ifndef _INCLUDE_LINUX_PLATFORM_DATA_DSI_BACKLIGHT_H
#define _INCLUDE_LINUX_PLATFORM_DATA_DSI_BACKLIGHT_H

#include <linux/spinlock.h>

struct mipi_dsi_device;
struct backlight_device;
struct dsi_backlight_platform_data;

typedef int (*mipi_dsi_backlight_init_cb)(struct dsi_backlight_platform_data *data);

struct dsi_backlight_platform_data {
	struct mipi_dsi_device *dsi;
	struct backlight_device *backlight;
	bool prepared;
	void *userdata;
};

#endif /* _INCLUDE_LINUX_PLATFORM_DATA_DSI_BACKLIGHT_H */
