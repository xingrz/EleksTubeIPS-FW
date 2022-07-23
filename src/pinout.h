#ifndef __PROJECT_PINOUT_H__
#define __PROJECT_PINOUT_H__

#include <zephyr.h>

#define RGB_NODE DT_ALIAS(rgb)
#define RGB_NUM_PIXELS DT_PROP(RGB_NODE, chain_length)

#define RTC_NODE DT_ALIAS(rtc)

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)

#endif  // __PROJECT_PINOUT_H__
