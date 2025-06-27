/*
 * led.h
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

#include "Drivers/pwm.h"

typedef union {
    uint32_t value;

    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    };
} rgb_color;

#define RGB(r, g, b) ((rgb_color){ .r = (r), .g = (g), .b = (b) })

#define RGB_HEX(hex) ((rgb_color){                    	\
    .r = (uint8_t)(((hex) >> 16) & 0xFF),               \
    .g = (uint8_t)(((hex) >> 8) & 0xFF),                \
    .b = (uint8_t)((hex) & 0xFF)                        \
})

static const rgb_color COLOR_RED     	= RGB_HEX(0xFF0000);
static const rgb_color COLOR_GREEN   	= RGB_HEX(0x00FF00);
static const rgb_color COLOR_BLUE    	= RGB_HEX(0x0000FF);
static const rgb_color COLOR_CYAN    	= RGB_HEX(0x00FFFF);
static const rgb_color COLOR_MAGENTA 	= RGB_HEX(0xFF00FF);
static const rgb_color COLOR_YELLOW  	= RGB_HEX(0xFFFF00);
static const rgb_color COLOR_ORANGE  	= RGB_HEX(0xFF8000);
static const rgb_color COLOR_PURPLE  	= RGB_HEX(0x800080);
static const rgb_color COLOR_WHITE   	= RGB_HEX(0xFFFFFF);
static const rgb_color COLOR_BLACK   	= RGB_HEX(0x000000);

#define COLOR_OFF COLOR_BLACK

rgb_color hsv_to_rgb(float h, float s, float v);

typedef struct {
	pwm_channel r_pwm;
	pwm_channel g_pwm;
	pwm_channel b_pwm;
} rgb_led;

void rgb_led_start(rgb_led* led);
void rgb_led_stop(rgb_led* led);

void rgb_led_set_color(rgb_led* led, rgb_color color);

#endif /* INC_LED_H_ */
