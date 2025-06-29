/*
 * servo.c
 *
 *  Created on: Jun 21, 2025
 *      Author: aravs
 */

#include "Drivers/servo.h"

void servo_start(servo* s) {
	s->angle = s->config.INIT_ANGLE;

	pwm_start(&s->pwm);
	servo_set_angle(s, s->config.INIT_ANGLE);
}

void servo_stop(servo* s) {
	pwm_stop(&s->pwm);
}

void servo_set_angle(servo* s, float angle) {
	if (angle > s->config.MAX_ANGLE) angle = s->config.MAX_ANGLE;
	if (angle < 0.0f) angle = 0.0f;

	s->angle = angle;

	const float angle_scaler = angle / s->config.MAX_ANGLE;
	const float delta_pw = s->config.MAX_PW - s->config.MIN_PW;

	float pulse = s->config.MIN_PW + angle_scaler * delta_pw;

	if (s->config.DIRECTION == SERVO_NEGATIVE) {
		pulse = s->config.MAX_PW - (pulse - s->config.MIN_PW);
	}

	uint32_t duty = (pulse / s->config.PERIOD) * s->pwm.resolution;

	pwm_set_duty(&s->pwm, duty);
}
