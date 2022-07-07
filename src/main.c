#include <device.h>
#include <drivers/counter.h>
#include <drivers/led_strip.h>
#include <drivers/rtc/maxim_ds3231.h>
#include <logging/log.h>
#include <zephyr.h>

#include "pinout.h"

LOG_MODULE_REGISTER(main);

static const struct device *rgb_dev = DEVICE_DT_GET(RGB_NODE);
static const struct device *rtc_dev = DEVICE_DT_GET(RTC_NODE);

void
main(void)
{
	if (!device_is_ready(rgb_dev)) {
		LOG_ERR("RGB LED device %s is not ready", rgb_dev->name);
		return;
	}

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

	LOG_INF("[DS3231] Sync clock freq %u Hz", maxim_ds3231_syncclock_frequency(rtc_dev));

	int stat = maxim_ds3231_stat_update(rtc_dev, 0, MAXIM_DS3231_REG_STAT_OSF);
	if (stat >= 0) {
		LOG_INF("[DS3231] Oscillator fault?: %lu", (stat & MAXIM_DS3231_REG_STAT_OSF));
	} else {
		LOG_INF("[DS3231] Stat fetch failed: %d", stat);
		return;
	}

	LOG_INF("[DS3231] Max top value: %u (%08x)", counter_get_max_top_value(rtc_dev),
		counter_get_max_top_value(rtc_dev));
	LOG_INF("[DS3231] Channels: %u", counter_get_num_of_channels(rtc_dev));
	LOG_INF("[DS3231] Freq: %u Hz", counter_get_frequency(rtc_dev));
	LOG_INF("[DS3231] Top counter value: %u (%08x)", counter_get_top_value(rtc_dev),
		counter_get_top_value(rtc_dev));

	LOG_INF("[DS3231] ctrl %02x ; ctrl_stat %02x", maxim_ds3231_ctrl_update(rtc_dev, 0, 0),
		maxim_ds3231_stat_update(rtc_dev, 0, 0));

	uint32_t now = 0;
	while (1) {
		counter_get_value(rtc_dev, &now);
		LOG_INF("[DS3231] time: %u", now);
		k_sleep(K_MSEC(1000));
	}
}
