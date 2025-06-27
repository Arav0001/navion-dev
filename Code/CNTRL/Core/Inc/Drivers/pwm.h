/*
 * pwm.h
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "main.h"

typedef struct {
	TIM_HandleTypeDef* htim;
	uint32_t channel;
	uint32_t resolution;
} pwm_channel;

void pwm_start(pwm_channel* pwm);
void pwm_stop(pwm_channel* pwm);
void pwm_set_duty(pwm_channel* pwm, uint32_t duty);

#endif /* INC_PWM_H_ */
