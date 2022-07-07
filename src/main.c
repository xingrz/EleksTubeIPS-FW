#include <device.h>
#include <drivers/gpio.h>
#include <drivers/led_strip.h>
#include <logging/log.h>
#include <zephyr.h>

#include "pinout.h"

LOG_MODULE_REGISTER(main);

static const struct device *rgb_dev = DEVICE_DT_GET(RGB_NODE);
static const struct gpio_dt_spec lcd_en = GPIO_DT_SPEC_GET(LCD_EN_NODE, gpios);

void
main(void)
{
	if (!device_is_ready(rgb_dev)) {
		LOG_ERR("RGB LED device %s is not ready", rgb_dev->name);
		return;
	}

	gpio_pin_configure_dt(&lcd_en, GPIO_OUTPUT_ACTIVE);

	struct led_rgb rgb[] = {
		{.r = 0xFF, .g = 0x00, .b = 0x00},
		{.r = 0x00, .g = 0xFF, .b = 0x00},
		{.r = 0x00, .g = 0x00, .b = 0xFF},
		{.r = 0xFF, .g = 0x00, .b = 0x00},
		{.r = 0x00, .g = 0xFF, .b = 0x00},
		{.r = 0x00, .g = 0x00, .b = 0xFF},
		{.r = 0xFF, .g = 0x00, .b = 0x00},
		{.r = 0x00, .g = 0xFF, .b = 0x00},
	};

	for (int i = 0; i < 30; i++) {
		led_strip_update_rgb(rgb_dev, &rgb[i % 3], RGB_NUM_PIXELS);
		k_sleep(K_MSEC(100));
	}
}
