/*
 * control.c
 *
 *  Created on: Jun 20, 2025
 *      Author: aravs
 */

#include "control.h"

void PID_init(PID* pid) {
	pid->pt = HAL_GetTick();
	pid->pe = 0.0f;
	pid->i = 0.0f;
	pid->d = 0.0f;
	pid->cfe = 0.0f;
	pid->pfe = 0.0f;
	pid->output = 0.0f;
}

void PID_compute(PID* pid, float target, float current) {
	uint32_t t = HAL_GetTick();

	float dt = ((float)(t - pid->pt)) / 1.0e3f;
	pid->pt = t;

	if (dt <= 0.0f) return;

	float error = target - current;

	pid->cfe = (pid->Ka * pid->pfe) + (1 - pid->Ka) * (error - pid->pe);
	pid->pfe = pid->cfe;

	pid->pe = error;

	pid->p = error;
	pid->i += error * dt;

	if (pid->Ki != 0.0f) {
	    float i_max = pid->OUT_MAX / pid->Ki;
	    float i_min = pid->OUT_MIN / pid->Ki;

		if (pid->i > i_max) pid->i = i_max;
	    if (pid->i < i_min) pid->i = i_min;
	}

	pid->d = pid->cfe / dt;

	float signal = pid->Kp * pid->p + pid->Ki * pid->i + pid->Kd * pid->d;

	if (signal > pid->OUT_MAX) signal = pid->OUT_MAX;
	if (signal < pid->OUT_MIN) signal = pid->OUT_MIN;

	pid->output = signal;
}
