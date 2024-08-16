//  STUB DRIVER FOR VERY DUMB AWINIC ANALOG AMPLIFIERS
//  THIS DRIVER IS MEANTS TO BE USED WITH "CONTAINERS" WHICH MEANS ENABLE SEQUENCES
//  THIS DRIVER ONLY IMPLEMENTS SINGLE CONTAINER SUPPORT AS MOST CONSUMERS FOR IT WILL BE PHONES 
//  WHERE VENDORS WONT GIVE A FLYING FUCK ABOUT SWITCHING IT FROM RECEIVER TO SPEAKER AND MUXING PINS AND WHATEVER ELSE
//  YOU CAN ALSO PASS CUSTOM INIT SEQUENCES VIA DT-BINDINGS BUT BETTER BE SURE TO DESCRIBE IT WELL OR KRZK GONNA YELL
 
#include <linux/i2c.h>
#include <linux/firmware.h>
#include <linux/regmap.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>
#include <sound/soc.h>

#define AW87XXX_ID_REG 0x0
#define AW87XXX_SYSCTRL_REG 0x1
#define AW87XXX_POWER_DOWN_VALUE 0
#define AW87XXX_CHIP_ID 0x59

static const struct regmap_config aw87xxx_remap_config = {
	.val_bits = 8,
	.reg_bits = 8,
	.max_register = 0xFF,
	.reg_format_endian = REGMAP_ENDIAN_LITTLE,
	.val_format_endian = REGMAP_ENDIAN_BIG,
};

struct aw87xxx_device {
	struct regulator *vddd_supply;
	struct i2c_client *i2c;
	struct device *dev;
	struct regmap *regmap;
};


static const struct reg_sequence aw87xxx_reg_init[] = {
{0x70, 0x80},
{0x1, 0x0},
{0x1, 0x0},
{0x2, 0xC},
{0x3, 0x6},
{0x4, 0x5},
{0x5, 0xC},
{0x6, 0x8},
{0x7, 0xB2},
{0x8, 0x9},
{0x9, 0x8},
{0xA, 0x4B},
{0x61, 0xBB},
{0x62, 0x80},
{0x63, 0x29},
{0x64, 0x58},
{0x65, 0xCD},
{0x66, 0x3C},
{0x67, 0x2F},
{0x68, 0x7},
{0x69, 0xDB},
{0x01, 0xD},
};

static int aw87xxx_init(struct regmap *regmap)
{
	return regmap_multi_reg_write(regmap, aw87xxx_reg_init,
				      ARRAY_SIZE(aw87xxx_reg_init));
}

static int aw87xxx_power_off(struct aw87xxx_device *aw_dev)
{
	int ret;
	unsigned int val;
	ret = regmap_read(aw_dev->regmap, AW87XXX_SYSCTRL_REG, &val);
	if (ret) {
		dev_err(aw_dev->dev, "failed to read status: %d\n", ret);
		return ret;
	}
	if ((val&0x1)==0) {
		dev_info(aw_dev->dev, "codec already software disabled\n");
		return 0;
	};

	ret = regmap_write(aw_dev->regmap, AW87XXX_SYSCTRL_REG, AW87XXX_POWER_DOWN_VALUE);
	if (ret)
		return ret;

	return 0;
}

static int aw87xxx_drv_event(struct snd_soc_dapm_widget *w,
				struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct aw87xxx_device *aw87xxx = snd_soc_component_get_drvdata(component);
	int ret;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
	aw87xxx_power_off(aw87xxx);
	ret = aw87xxx_init(aw87xxx->regmap);
		break;
	case SND_SOC_DAPM_POST_PMD:
		ret = aw87xxx_power_off(aw87xxx);
		break;
	default:
		dev_err(aw87xxx->dev, "%s: invalid event %d\n", __func__, event);
		ret = -EINVAL;
	}

	return ret;
}

static const struct snd_soc_dapm_widget aw87xxx_dapm_widgets[] = {
	SND_SOC_DAPM_INPUT("IN"),
	SND_SOC_DAPM_OUT_DRV_E("SPK PA", SND_SOC_NOPM, 0, 0, NULL, 0, aw87xxx_drv_event,
			       SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUTPUT("OUT"),
};

static const struct snd_soc_dapm_route aw87xxx_dapm_routes[] = {
	{ "SPK PA", NULL, "IN" },
	{ "OUT", NULL, "SPK PA" },
};

static const struct snd_soc_component_driver soc_codec_dev_aw87xxx = {
	//.probe = aw87xxx_codec_probe,
	.dapm_widgets = aw87xxx_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(aw87xxx_dapm_widgets),
	.dapm_routes = aw87xxx_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(aw87xxx_dapm_routes),
	//.controls = aw87xxx_controls,
	//.num_controls = ARRAY_SIZE(aw87xxx_controls),
};

static void aw87xxx_regulator_disable(void *data)
{
	struct aw87xxx_device *aw87xxx = data;

	regulator_disable(aw87xxx->vddd_supply);
}

static int aw87xxx_i2c_probe(struct i2c_client *i2c)
{	
	struct device *dev = &i2c->dev;
	struct aw87xxx_device *aw87xxx;
	struct regmap *regmap;
	unsigned int val;
	int ret;
 /*
	ret = i2c_check_functionality(i2c->adapter, I2C_FUNC_I2C);
	if (!ret)
		return dev_err_probe(&i2c->dev, -ENXIO, "check_functionality failed\n");
*/
	aw87xxx = devm_kzalloc(dev, sizeof(*aw87xxx), GFP_KERNEL);
	if (!aw87xxx)
		return -ENOMEM;

	i2c_set_clientdata(i2c, aw87xxx);
	
	aw87xxx->vddd_supply = devm_regulator_get(dev, "vddd");
	if (IS_ERR(aw87xxx->vddd_supply))
		return dev_err_probe(dev, PTR_ERR(aw87xxx->vddd_supply),
				     "Failed to get vddd regulator\n");
				     
	
	aw87xxx->regmap = devm_regmap_init_i2c(i2c, &aw87xxx_remap_config);
	if (IS_ERR(aw87xxx->regmap))
		return dev_err_probe(&i2c->dev, PTR_ERR(aw87xxx->regmap),
					"failed to init regmap\n");

	ret = regulator_enable(aw87xxx->vddd_supply);
	if (ret) {
		dev_err(dev, "Failed to enable vddd regulator: %d\n", ret);
		return ret;
	}
	
	ret = devm_add_action_or_reset(dev, aw87xxx_regulator_disable, aw87xxx);
	if (ret)
		return ret;
		
	regcache_cache_bypass(regmap, true);
	
	
	/* Dummy read to generate i2c clocks, required on some devices */
	regmap_read(regmap, AW87XXX_ID_REG, &val);
	
	ret = regmap_read(regmap, AW87XXX_ID_REG, &val);
	if (ret) {
		dev_err(dev, "failed to read revision number: %d\n", ret);
		return ret;
	}

	if (val!= AW87XXX_CHIP_ID) {
		dev_err(&i2c->dev, "unsupported device\n");
		return -ENXIO;
	}
	/* Power down amp, each sequence init should also turn it off and after configuring on but we want to be sure */
	/*OFC reg should be passed via DT because we want to support even ones with shuffled registers or pass it in init data whatever i don't care at this point */
	ret = regmap_write(regmap, AW87XXX_SYSCTRL_REG, AW87XXX_POWER_DOWN_VALUE);
	if (ret) {
		dev_err(dev, "failed to power down amp: %d\n", ret);
		return ret;
	}

regcache_cache_bypass(regmap, false);
	return devm_snd_soc_register_component(&i2c->dev,
				&soc_codec_dev_aw87xxx, NULL, 0);
}

static const struct of_device_id aw87xxx_of_match[] = {
	{ .compatible = "awinic,aw87xxx" },
	{ }
};

MODULE_DEVICE_TABLE(of, aw87xxx_of_match);

static struct i2c_driver aw87xxx_i2c_driver = {
	.driver = {
		.name = "aw87xxx",
		.of_match_table = aw87xxx_of_match,
	},
	.probe = aw87xxx_i2c_probe,
};
module_i2c_driver(aw87xxx_i2c_driver);


MODULE_DESCRIPTION("ASoC aw87xxx PA Driver");
MODULE_LICENSE("GPL");
