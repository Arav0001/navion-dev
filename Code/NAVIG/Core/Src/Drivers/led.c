/*
 * led.c
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#include "Drivers/led.h"

#include <math.h>

void rgb_led_start(rgb_led* led) {
    pwm_start(&led->r_pwm);
    pwm_start(&led->g_pwm);
    pwm_start(&led->b_pwm);

    rgb_led_set_color(led, COLOR_OFF);
}

void rgb_led_stop(rgb_led* led) {
	pwm_stop(&led->r_pwm);
	pwm_stop(&led->g_pwm);
	pwm_stop(&led->b_pwm);
}

rgb_color hsv_to_rgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r1 = 0, g1 = 0, b1 = 0;

    if (h < 60.0f) {
        r1 = c; g1 = x; b1 = 0;
    } else if (h < 120.0f) {
        r1 = x; g1 = c; b1 = 0;
    } else if (h < 180.0f) {
        r1 = 0; g1 = c; b1 = x;
    } else if (h < 240.0f) {
        r1 = 0; g1 = x; b1 = c;
    } else if (h < 300.0f) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    rgb_color color;
    color.r = (uint8_t)((r1 + m) * 255.0f);
    color.g = (uint8_t)((g1 + m) * 255.0f);
    color.b = (uint8_t)((b1 + m) * 255.0f);
    return color;
}

void rgb_led_set_color(rgb_led* led, rgb_color color) {
	uint32_t r_duty = (color.r * led->r_pwm.resolution) / 255;
    uint32_t g_duty = (color.g * led->g_pwm.resolution) / 255;
    uint32_t b_duty = (color.b * led->b_pwm.resolution) / 255;

    pwm_set_duty(&led->r_pwm, r_duty);
    pwm_set_duty(&led->g_pwm, g_duty);
    pwm_set_duty(&led->b_pwm, b_duty);
}
