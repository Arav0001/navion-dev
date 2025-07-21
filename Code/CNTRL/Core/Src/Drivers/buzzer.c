/*
 * buzzer.c
 *
 *  Created on: Jul 20, 2025
 *      Author: aravs
 */

#include "Drivers/buzzer.h"

static uint8_t state = 0;

void buzzer_init(buzzer* bzr) {
    pwm_start(&bzr->pwm);
}

void buzzer_set_volume(buzzer* bzr, float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    uint32_t duty = (uint32_t)(volume * bzr->pwm.resolution);
    pwm_set_duty(&bzr->pwm, duty);
}

void buzzer_stop(buzzer* bzr) {
    pwm_set_duty(&bzr->pwm, 0);
    pwm_stop(&bzr->pwm);
}
