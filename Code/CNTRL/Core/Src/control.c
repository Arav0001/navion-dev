/*
 * control.c
 *
 *  Created on: Jun 20, 2025
 *      Author: aravs
 */

#include "control.h"

void PID_init(PID* pid) {
	pid->last_ms = HAL_GetTick();
}

void PID_compute(PID* pid, float target, float current) {
	float curr_ms = HAL_GetTick();

	float dt = (curr_ms - pid->last_ms) / 1000.0f;
	if (dt <= 0.0f) dt = 0.001f;

	float error = target - current;

	pid->integral += error * dt;

	float derivative = (current - pid->last_measure) / dt;

	float output =	pid->Kp * error +
					pid->Ki * pid->integral +
					pid->Kd * derivative;

	if (output > pid->OUT_MAX) output = pid->OUT_MAX;
	if (output < pid->OUT_MIN) output = pid->OUT_MIN;

	pid->last_measure = current;
	pid->last_ms = curr_ms;
	pid->output = output;
}
