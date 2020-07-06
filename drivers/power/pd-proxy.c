/*
 * Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/pm_opp.h>
#include <linux/pm_runtime.h>

struct proxy_pd {
	struct device		*dev;
	struct mutex		lock;
	struct generic_pm_domain genpd;
	bool			enabled;
	u32			level;
	u32			num_pds;
	u32			attached_cnt;
	struct device		*pds[];
};

static int pd_proxy_power_on(struct generic_pm_domain *domain)
{
	struct proxy_pd *proxy = container_of(domain, struct proxy_pd, genpd);
	int ret, i;

	mutex_lock(&proxy->lock);

	if (proxy->enabled)
		goto enabled;

	for (i = 0; i < proxy->num_pds; i++) {
		if (!proxy->pds[i])
			continue;

		if (i == 0 && proxy->level)
			dev_pm_genpd_set_performance_state(proxy->pds[i], proxy->level);

		ret = pm_runtime_get_sync(proxy->pds[i]);
		if (ret < 0)
			goto fail;

		pm_runtime_forbid(proxy->pds[i]);
	}

	proxy->enabled = true;

enabled:
	mutex_unlock(&proxy->lock);

	return 0;

fail:
	for (i--; i >= 0; i--) {
		pm_runtime_allow(proxy->pds[i]);
		pm_runtime_put(proxy->pds[i]);
	}
	dev_pm_genpd_set_performance_state(proxy->pds[0], 0);
	mutex_unlock(&proxy->lock);
	return ret;
}

int pm_proxy_attach_dev(struct generic_pm_domain *domain,
		struct device *dev)
{
	struct proxy_pd *proxy = container_of(domain, struct proxy_pd, genpd);
	mutex_lock(&proxy->lock);
	proxy->attached_cnt++;
	mutex_unlock(&proxy->lock);
	return 0;
}

void pm_proxy_detach_dev(struct generic_pm_domain *domain,
		struct device *dev)
{
	struct proxy_pd *proxy = container_of(domain, struct proxy_pd, genpd);
	mutex_lock(&proxy->lock);
	if (proxy->attached_cnt)
		proxy->attached_cnt--;
	mutex_unlock(&proxy->lock);
}


static int pd_proxy_power_off(struct generic_pm_domain *domain)
{
	struct proxy_pd *proxy = container_of(domain, struct proxy_pd, genpd);
	int i;

	mutex_lock(&proxy->lock);

	if (!proxy->enabled || (proxy->attached_cnt == 0))
		goto disabled;

	for (i = proxy->num_pds - 1; i >= 0; i--) {
		pm_runtime_allow(proxy->pds[i]);
		pm_runtime_put(proxy->pds[i]);
		if (i == 0 && proxy->level)
			dev_pm_genpd_set_performance_state(proxy->pds[i], 0);
	}

	proxy->enabled = false;

disabled:
	mutex_unlock(&proxy->lock);

	return 0;
}

static int pd_proxy_set_performance_state(struct generic_pm_domain *domain,
				     unsigned int level)
{
	struct proxy_pd *proxy = container_of(domain, struct proxy_pd, genpd);

	mutex_lock(&proxy->lock);

	proxy->level = level;

	if (proxy->enabled && proxy->num_pds)
		dev_pm_genpd_set_performance_state(proxy->pds[0], level);

	mutex_unlock(&proxy->lock);

	return 0;
}

static int pd_proxy_attach_domains(struct proxy_pd *proxy)
{
	int i, ret = 0;
	for (i = 0; i < proxy->num_pds; i++) {
		if (proxy->pds[i])
			continue;

		mutex_lock(&proxy->lock);
		proxy->pds[i] = dev_pm_domain_attach_by_id(proxy->dev, i);
		if (IS_ERR_OR_NULL(proxy->pds[i])) {
			ret = -EPROBE_DEFER;
			proxy->pds[i] = NULL;
		}
		mutex_unlock(&proxy->lock);

	}

	return ret;
}

static void pd_proxy_detach_domains(struct proxy_pd *proxy, bool power_off)
{
	int i;
	for (i = 0; i < proxy->num_pds; i++)
		if (proxy->pds[i])
			dev_pm_domain_detach(proxy->pds[i], power_off);
}

static int pd_proxy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct proxy_pd *proxy;
	int ret, num_pds;
	bool is_off = true;
	int type = (long) of_device_get_match_data(&pdev->dev);

	num_pds = of_property_count_strings(dev->of_node, "power-domain-names");
	if (num_pds <= 0)
		return -EINVAL;

	proxy = devm_kzalloc(dev, sizeof(*proxy) + num_pds * sizeof(proxy->pds[0]),
			     GFP_KERNEL);

	if (!proxy)
		return -ENOMEM;

	mutex_init(&proxy->lock);

	platform_set_drvdata(pdev, proxy);

	proxy->dev = &pdev->dev;
	proxy->enabled = false;
	proxy->num_pds = num_pds;

	if (dev->pm_domain) {
		proxy->pds[0] = dev;
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	} else {
		ret = pd_proxy_attach_domains(proxy);
		if (ret) {
			pd_proxy_detach_domains(proxy, false);
			return ret;
		}
	}

	proxy->genpd.name = devm_kstrdup_const(dev, dev->of_node->full_name, GFP_KERNEL);
	if (!proxy->genpd.name) {
		ret = -ENOMEM;
		goto fail;
	}

	proxy->genpd.power_on = pd_proxy_power_on;
	proxy->genpd.power_off = pd_proxy_power_off;
	if (of_property_read_bool(dev->of_node, "operating-points-v2"))
		proxy->genpd.set_performance_state = pd_proxy_set_performance_state;

	if (of_property_read_bool(dev->of_node, "force-on-unused")) {
		proxy->genpd.attach_dev = pm_proxy_attach_dev;
		proxy->genpd.detach_dev = pm_proxy_detach_dev;
		pd_proxy_power_on(&proxy->genpd);
		is_off = false;
	} else {
		proxy->attached_cnt = 1;
	}

	ret = pm_genpd_init(&proxy->genpd, NULL, is_off);
	if (ret < 0)
		goto fail;

	if (type & 0x1) {
		// Some power-domains need IRQs to change state (rpmpd), so
		// they can't be disabled/enabled during suspend_noirq/resume_noirq
		proxy->genpd.domain.ops.resume_early = proxy->genpd.domain.ops.resume_noirq;
		proxy->genpd.domain.ops.suspend_late = proxy->genpd.domain.ops.suspend_noirq;;
		proxy->genpd.domain.ops.resume_noirq = NULL;
		proxy->genpd.domain.ops.suspend_noirq = NULL;
	}

	ret = of_genpd_add_provider_simple(dev->of_node, &proxy->genpd);
	if (ret < 0)
		goto fail;

	return 0;

fail:
	pd_proxy_detach_domains(proxy, false);
	return ret;
}

static int pd_proxy_remove(struct platform_device *pdev)
{
	struct proxy_pd *proxy = platform_get_drvdata(pdev);

	pd_proxy_power_off(&proxy->genpd);

	pd_proxy_detach_domains(proxy, false);

	of_genpd_del_provider(pdev->dev.of_node);

	pm_genpd_remove(&proxy->genpd);

	return 0;
}

static const struct of_device_id pd_proxy_match_table[] = {
	{ .compatible = "pd-proxy", .data = (void*) 0x0 },
	{ .compatible = "pd-proxy-noirq-fix", .data = (void*) 0x1 },
	{}
};

static struct platform_driver pd_proxy_pd_driver = {
	.driver	= {
		.name		= "pd-proxy",
		.of_match_table = pd_proxy_match_table,
		.owner		= THIS_MODULE,
	},
	.probe	= pd_proxy_probe,
	.remove	= pd_proxy_remove,
};

module_platform_driver(pd_proxy_pd_driver);

MODULE_DESCRIPTION("Power domain proxy driver");
MODULE_LICENSE("GPL v2");
