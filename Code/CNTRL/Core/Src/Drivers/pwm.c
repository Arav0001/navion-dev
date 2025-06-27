/*
 * pwm.c
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#include "Drivers/pwm.h"

void pwm_start(pwm_channel* pwm) {
    HAL_TIM_PWM_Start(pwm->htim, pwm->channel);
}

void pwm_stop(pwm_channel* pwm) {
    HAL_TIM_PWM_Stop(pwm->htim, pwm->channel);
}

void pwm_set_duty(pwm_channel* pwm, uint32_t duty) {
    if (duty > pwm->resolution) duty = pwm->resolution;
    __HAL_TIM_SET_COMPARE(pwm->htim, pwm->channel, duty);
}
