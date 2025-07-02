/*
 * orientation.c
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#include "orientation.h"
#include "util.h"

#include <string.h>

extern sensor_data data;

float last_time;
float current_time;
float dt;

float orientation_freq;

void initialize_orientation() {
	q0 = 0.0f;
	q1 = -0.5f * sqrt(2.0f);
	q2 = -q1;
	q3 = 0.0f;
}

void calculate_orientation(float* quaternion, float* roll, float* pitch, float* yaw) {
	last_time = current_time;
	current_time = HAL_GetTick();
	dt = current_time - last_time;
	orientation_freq = 1.0f / dt;

	MadgwickAHRSupdate(data.gx, data.gy, data.gz, data.ax, data.ay, data.az, data.mx, data.my, data.mz);

	quaternion[0] = q0;
	quaternion[1] = q1;
	quaternion[2] = q2;
	quaternion[3] = q3;

	*roll = RAD_TO_DEG * -atan2f(2.0f * (q0*q3 + q1*q2), 1.0f - 2.0f * (q2*q2 + q3*q3));
	*yaw = RAD_TO_DEG * -asinf(2.0f * (q0*q2 - q3*q1));

	float c_pitch = RAD_TO_DEG * atan2f(2.0f * (q0*q1 + q2*q3), 1.0f - 2.0f * (q1*q1 + q2*q2));

	if (c_pitch > 0.0f) {
		*pitch = 180.0f - c_pitch;
	} else if (c_pitch < 0.0f) {
		*pitch = -180.0f - c_pitch;
	} else {
		*pitch = 0.0f;
	}
}
