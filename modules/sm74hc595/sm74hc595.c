#define DT_DRV_COMPAT sunmoon_sm74hc595

#include <drivers/gpio.h>
#include <logging/log.h>
#include <stdint.h>
#include <zephyr.h>

LOG_MODULE_REGISTER(sm74hc595);

#define NGPIOS 8
#define PULSE_WIDTH_NS 1

struct sm74hc595_data {
	struct gpio_driver_data common;
	uint8_t states[NGPIOS];
};

struct sm74hc595_config {
	struct gpio_driver_config drv_cfg;
	struct gpio_dt_spec ser_gpio;
	struct gpio_dt_spec sck_gpio;
	struct gpio_dt_spec rck_gpio;
	int init_states;
};

static int
sm74hc595_get_raw(const struct device *dev, gpio_port_value_t *value)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;

	gpio_port_value_t val = 0;
	for (int i = 0; i < NGPIOS; i++) {
		val |= data->states[i] << i;
	}
	*value = val;

	return 0;
}

static int
_sm74hc595_update_state(const struct device *dev)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;
	int ret;

	for (int i = NGPIOS - 1; i >= 0; i--) {
		ret = gpio_pin_set_dt(&config->ser_gpio, data->states[i]);
		if (ret != 0) return ret;

		ret = gpio_pin_set_dt(&config->sck_gpio, 1);
		if (ret != 0) return ret;

		k_usleep(PULSE_WIDTH_NS);

		ret = gpio_pin_set_dt(&config->sck_gpio, 0);
		if (ret != 0) return ret;
	}

	ret = gpio_pin_set_dt(&config->rck_gpio, 1);
	if (ret != 0) return ret;

	k_usleep(PULSE_WIDTH_NS);

	ret = gpio_pin_set_dt(&config->rck_gpio, 0);
	if (ret != 0) return ret;

	return 0;
}

static int
sm74hc595_set_marked_raw(const struct device *dev, gpio_port_pins_t mask, gpio_port_value_t value)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;

	for (int i = 0; i < NGPIOS; i++) {
		if ((mask >> i) & 0x1) {
			data->states[i] = (mask >> i) & 0x1;
		}
	}

	return _sm74hc595_update_state(dev);
}

static int
sm74hc595_set_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;

	for (int i = 0; i < NGPIOS; i++) {
		if ((pins >> i) & 0x1) {
			data->states[i] = 1;
		}
	}

	return _sm74hc595_update_state(dev);
}

static int
sm74hc595_clear_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;

	for (int i = 0; i < NGPIOS; i++) {
		if ((pins >> i) & 0x1) {
			data->states[i] = 0;
		}
	}

	return _sm74hc595_update_state(dev);
}

static int
sm74hc595_toggle_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;

	for (int i = 0; i < NGPIOS; i++) {
		if ((pins >> i) & 0x1) {
			data->states[i] = !data->states[i];
		}
	}

	return _sm74hc595_update_state(dev);
}

static int
sm74hc595_init(const struct device *dev)
{
	const struct sm74hc595_config *config = dev->config;
	struct sm74hc595_data *data = dev->data;
	int ret;

	if (!device_is_ready(config->ser_gpio.port)) {
		LOG_ERR("SER GPIO not ready");
		return -ENODEV;
	}

	if (!device_is_ready(config->sck_gpio.port)) {
		LOG_ERR("SCK GPIO not ready");
		return -ENODEV;
	}

	if (!device_is_ready(config->rck_gpio.port)) {
		LOG_ERR("RCK GPIO not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->ser_gpio, GPIO_OUTPUT_HIGH);
	if (ret < 0) {
		LOG_ERR("Could not configure SER GPIO (%d)", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&config->sck_gpio, GPIO_OUTPUT_LOW);
	if (ret < 0) {
		LOG_ERR("Could not configure SCK GPIO (%d)", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&config->rck_gpio, GPIO_OUTPUT_LOW);
	if (ret < 0) {
		LOG_ERR("Could not configure RCK GPIO (%d)", ret);
		return ret;
	}

	memset(data->states, 0, NGPIOS);

	return sm74hc595_set_bits_raw(dev, config->init_states);
}

static const struct gpio_driver_api sm74hc595_gpio_api = {
	.port_get_raw = sm74hc595_get_raw,
	.port_set_masked_raw = sm74hc595_set_marked_raw,
	.port_set_bits_raw = sm74hc595_set_bits_raw,
	.port_clear_bits_raw = sm74hc595_clear_bits_raw,
	.port_toggle_bits = sm74hc595_toggle_bits_raw,
};

#define SM74HC595_DEVICE(i)                                                                     \
	static struct sm74hc595_data sm74hc595_data_##i;                                            \
                                                                                                \
	static const struct sm74hc595_config sm74hc595_config_##i = {                               \
		.ser_gpio = GPIO_DT_SPEC_INST_GET(i, ser_gpios),                                        \
		.sck_gpio = GPIO_DT_SPEC_INST_GET(i, sck_gpios),                                        \
		.rck_gpio = GPIO_DT_SPEC_INST_GET(i, rck_gpios),                                        \
		.init_states = DT_INST_PROP_OR(i, init_states, 0),                                      \
	};                                                                                          \
                                                                                                \
	DEVICE_DT_INST_DEFINE(i, &sm74hc595_init, NULL, &sm74hc595_data_##i, &sm74hc595_config_##i, \
		POST_KERNEL, CONFIG_SERIAL_INIT_PRIORITY, &sm74hc595_gpio_api);

DT_INST_FOREACH_STATUS_OKAY(SM74HC595_DEVICE)
