#ifndef __PROJECT_PINOUT_H__
#define __PROJECT_PINOUT_H__

#include <zephyr.h>

#define RGB_NODE DT_ALIAS(rgb)
#define RGB_NUM_PIXELS DT_PROP(RGB_NODE, chain_length)

#define RTC_NODE DT_ALIAS(rtc)

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)
#define DISPLAY_WIDTH DT_PROP(DISPLAY_NODE, width)
#define DISPLAY_HEIGHT DT_PROP(DISPLAY_NODE, height)
#define DISPLAY_NUM_SCREENS DT_PROP(DISPLAY_NODE, screens)
#define DISPLAY_SCREEN_WIDTH (DISPLAY_WIDTH / DISPLAY_NUM_SCREENS)

#endif  // __PROJECT_PINOUT_H__
