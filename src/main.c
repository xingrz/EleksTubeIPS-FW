#include <device.h>
#include <drivers/counter.h>
#include <drivers/display.h>
#include <drivers/gpio.h>
#include <drivers/led_strip.h>
#include <drivers/rtc/maxim_ds3231.h>
#include <logging/log.h>
#include <lvgl.h>
#include <zephyr.h>

#include "pinout.h"

LOG_MODULE_REGISTER(main);

static const struct device *rgb_dev = DEVICE_DT_GET(RGB_NODE);
static const struct device *rtc_dev = DEVICE_DT_GET(RTC_NODE);
static const struct device *display_dev = DEVICE_DT_GET(DISPLAY_NODE);
static const struct device *gpio_ex_dev = DEVICE_DT_GET(DT_NODELABEL(gpio_ex));

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

	char count_str[11] = {0};
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	lv_obj_t *rect = lv_obj_create(lv_scr_act());
	lv_obj_set_pos(rect, 0, 0);
	lv_obj_set_size(rect, 135, 240);
	lv_obj_set_style_bg_color(rect, lv_color_make(0x00, 0x00, 0x00), LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(rect, 0, LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(rect, 0, LV_STATE_DEFAULT);

	lv_obj_t *count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_text_font(count_label, &lv_font_montserrat_48, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(count_label, lv_color_make(0xff, 0xff, 0xff), LV_STATE_DEFAULT);
	lv_label_set_text(count_label, "0");

	lv_task_handler();
	display_blanking_off(display_dev);

	gpio_port_set_masked_raw(gpio_ex_dev, 0x3f, 0x3f);

	uint32_t now = 0;
	while (1) {
		counter_get_value(rtc_dev, &now);

		sprintf(count_str, "%u", now);
		char *start = count_str + strlen(count_str) - 6;
		for (int i = 5; i >= 0; i--) {
			gpio_pin_set(gpio_ex_dev, i, 0);
			start[i + 1] = '\0';
			lv_label_set_text(count_label, start + i);
			lv_task_handler();
			gpio_pin_set(gpio_ex_dev, i, 1);
			k_sleep(K_MSEC(20));
		}
	}
}
