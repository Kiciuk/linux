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

struct init_data {
struct reg_sequence *aw87xxx_reg_init;
//struct reg_sequence *aw87xxx_reg_override;
int reg_seq_size;
unsigned int aw87xxx_version;
};

enum {
	AWINIC_AW87359,
	AWINIC_AW87519,
};

struct aw87xxx_device {
	struct regulator *vddd_supply;
	struct i2c_client *i2c;
	struct device *dev;
	struct regmap *regmap;
	struct init_data *data;
	struct gpio_desc *reset_gpiod;
};

//dspk
//abspk
static const struct reg_sequence aw87359_reg_init[] = {

{0x70, 0x80},
	{0x01, 0x00},
	{0x01, 0x00},
	{0x02, 0x0C},
	{0x03, 0x08},
	{0x04, 0x05},
	{0x05, 0x10},
	{0x06, 0x07},
	{0x07, 0x4E},
	{0x08, 0x06},
	{0x09, 0x08},
	{0x0A, 0x4A},
	{0x61, 0xBB},
	{0x62, 0x80},
	{0x63, 0x29},
	{0x64, 0x58},
	{0x65, 0xCD},
	{0x66, 0x3C},
	{0x67, 0x2F},
	{0x68, 0x07},
	{0x69, 0xDB},
	{0x01, 0x0D},
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


//kspk custom maybe we need stock?
/*
static const struct reg_sequence aw87519_reg_override_init[] = {
{0x69, 0xB7},
{0x69, 0xB7},
{0x2, 0x9},
{0x3, 0xE8},
{0x4, 0x11},
{0x5, 0x0C},
{0x6, 0x4B},
{0x7, 0xB6},
{0x8, 0x0B},
{0x9, 0x08},
{0xA, 0x4B},
{0x60, 0x16},
{0x61, 0x20},
{0x62, 0x01},
{0x63, 0x0B},
{0x64, 0xC5},
{0x65, 0xA4},
{0x66, 0x78},
{0x67, 0xC4},
{0x68, 0x90},
{0x01, 0xF0},
};
*/
//defaults now
static const struct reg_sequence aw87519_reg_init[] = {

{0x69, 0x80},
	{0x69, 0xB7},
	{0x01, 0xF0},
	{0x02, 0x09},
	{0x03, 0xE8},
	{0x04, 0x11},
	{0x05, 0x10},
	{0x06, 0x43},
	{0x07, 0x4E},
	{0x08, 0x03},
	{0x09, 0x08},
	{0x0A, 0x4A},
	{0x60, 0x16},
	{0x61, 0x20},
	{0x62, 0x01},
	{0x63, 0x0B},
	{0x64, 0xC5},
	{0x65, 0xA4},
	{0x66, 0x78},
	{0x67, 0xC4},
	{0x68, 0X90},
	{0x69, 0xB7},
{0x69, 0xB7},
{0x2, 0x9},
{0x3, 0xE8},
{0x4, 0x11},
{0x5, 0x0C},
{0x6, 0x4B},
{0x7, 0xB6},
{0x8, 0x0B},
{0x9, 0x08},
{0xA, 0x4B},
{0x60, 0x16},
{0x61, 0x20},
{0x62, 0x01},
{0x63, 0x0B},
{0x64, 0xC5},
{0x65, 0xA4},
{0x66, 0x78},
{0x67, 0xC4},
{0x68, 0x90},
{0x01, 0xF0},
};

static const struct init_data aw87359_init = {
.aw87xxx_reg_init = aw87359_reg_init,
.reg_seq_size = ARRAY_SIZE(aw87359_reg_init),
.aw87xxx_version = AWINIC_AW87359,
};

static const struct init_data aw87519_init = {
.aw87xxx_reg_init = aw87519_reg_init,
.reg_seq_size = ARRAY_SIZE(aw87519_reg_init),
.aw87xxx_version = AWINIC_AW87519,
};


static int aw87xxx_init(struct aw87xxx_device *aw_device)
//static int aw87xxx_init(struct regmap *regmap)
{
int ret;
u32 val;

ret = regmap_multi_reg_write(aw_device->regmap, aw_device->data->aw87xxx_reg_init,
				     aw_device->data->reg_seq_size);
	printk("aw87xxx_init returned %d",ret);
	return ret;
}

static int aw87519_read_chipid(struct aw87xxx_device *aw_dev)
{
	int ret;
	int val = 0;
	unsigned int count = 0;
	while (count< 10) {
		ret = regmap_write(aw_dev->regmap, 0x64, 0x2C);
		ret = regmap_read(aw_dev->regmap, AW87XXX_ID_REG, &val);
			if (val == 0x59)
			{
				printk("managed to read chip at %d",count);
				return 0;
			}
		count++;
		printk("[aw] failed to read keep going");
		usleep_range(2000,2500);
	};

	return -EINVAL;
};

void aw87xxx_reg_dump(struct aw87xxx_device *aw_device) 
{
u32 val =0;

regmap_read(aw_device->regmap, 0x70, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x1, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x2, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x3, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x4, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x5, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x6, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x7, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x8, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x9, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0xA, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x61, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x62, &val);
printk("value: 0x%x",val);
regmap_read(aw_device->regmap, 0x1, &val);
printk("Enable value: 0x%x",val);
};


static int aw87xxx_hw_reset(struct aw87xxx_device *aw_dev) 
{
printk("resseting gpios");
gpiod_set_value_cansleep(aw_dev->reset_gpiod, 0);
usleep_range(1000, 2000);
gpiod_set_value_cansleep(aw_dev->reset_gpiod, 1);
usleep_range(9000, 11000);

return 0;
};

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
	//aw87xxx_power_off(aw87xxx);
	printk("[aw]initing via regmap version:%d",aw87xxx->data->aw87xxx_version);
	ret = aw87xxx_init(aw87xxx);
	printk("[aw]inited regs");
		break;
	case SND_SOC_DAPM_POST_PMD:
		//ret = aw87xxx_power_off(aw87xxx);
		printk("[aw]post_pmd");
		break;
	default:
		dev_err(aw87xxx->dev, "%s: invalid event %d\n", __func__, event);
		ret = -EINVAL;
	}

	return ret;
}

static const struct snd_soc_dapm_widget aw87xxx_dapm_widgets[] = {
	SND_SOC_DAPM_INPUT("INN"),
	SND_SOC_DAPM_OUT_DRV_E("SPK PA", SND_SOC_NOPM, 0, 0, NULL, 0, aw87xxx_drv_event,
			       SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUTPUT("OUT"),
};

static const struct snd_soc_dapm_route aw87xxx_dapm_routes[] = {
	{ "SPK PA", NULL, "INN" },
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
	struct init_data *pdata;
	unsigned int val;
	int ret;

	ret = i2c_check_functionality(i2c->adapter, I2C_FUNC_I2C);
	if (!ret)
		return dev_err_probe(&i2c->dev, -ENXIO, "check_functionality failed\n");

	aw87xxx = devm_kzalloc(dev, sizeof(*aw87xxx), GFP_KERNEL);
	if (!aw87xxx)
		return -ENOMEM;

	i2c_set_clientdata(i2c, aw87xxx);
	
	
	aw87xxx->data = device_get_match_data(dev);
	
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
	
	aw87xxx->reset_gpiod = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
		if (IS_ERR(aw87xxx->reset_gpiod))
			return PTR_ERR(aw87xxx->reset_gpiod);
	
	
	ret = devm_add_action_or_reset(dev, aw87xxx_regulator_disable, aw87xxx);
	if (ret)
		return ret;
	printk("awinic version: %d",aw87xxx->data->aw87xxx_version);
if(aw87xxx->data->aw87xxx_version == AWINIC_AW87519 )
	{
	printk("executing aw87519 flow");
		aw87xxx_hw_reset(aw87xxx);
		ret = aw87519_read_chipid(aw87xxx);
	}
	else
	ret = regmap_read(aw87xxx->regmap, AW87XXX_ID_REG, &val);
	
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
	ret = regmap_write(aw87xxx->regmap, AW87XXX_SYSCTRL_REG, AW87XXX_POWER_DOWN_VALUE);
	if (ret) {
		dev_err(dev, "failed to power down amp: %d\n", ret);
		return ret;
	}
	ret = aw87xxx_init(aw87xxx);
	if (ret) {
		dev_err(dev, "failed to init: %d\n", ret);
		return ret;
	}

	return devm_snd_soc_register_component(&i2c->dev,
				&soc_codec_dev_aw87xxx, NULL, 0);
}

static const struct of_device_id aw87xxx_of_match[] = {
	{ .compatible = "awinic,aw87359", .data = &aw87359_init },
	{ .compatible = "awinic,aw87519", .data = &aw87519_init },
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
