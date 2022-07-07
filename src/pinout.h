#ifndef __PROJECT_PINOUT_H__
#define __PROJECT_PINOUT_H__

#include <zephyr.h>

#define RGB_NODE DT_ALIAS(rgb)
#define RGB_NUM_PIXELS DT_PROP(RGB_NODE, chain_length)

#define LCD_EN_NODE DT_ALIAS(lcd_en)

#endif  // __PROJECT_PINOUT_H__
