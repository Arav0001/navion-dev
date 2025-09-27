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

extern uint8_t calibration_set;

void initialize_orientation() {
	q0 = 0.0f;
	q1 = -0.5f * sqrt(2.0f);
	q2 = -q1;
	q3 = 0.0f;
}

void calculate_orientation(float* quaternion, float* quat0, float* roll, float* pitch, float* yaw) {
	last_time = current_time;
	current_time = HAL_GetTick();
	dt = current_time - last_time;
	orientation_freq = 1.0f / dt;

	MadgwickAHRSupdateIMU(data.gx, data.gy, data.gz, data.ax, data.ay, data.az);

	float temp_quat[4] = {0};

	temp_quat[0] = q0;
	temp_quat[1] = q1;
	temp_quat[2] = q2;
	temp_quat[3] = q3;

	if (calibration_set) {
		quat_relative(quat0, temp_quat, quaternion);
	} else {
		quaternion[0] = temp_quat[0];
		quaternion[1] = temp_quat[1];
		quaternion[2] = temp_quat[2];
		quaternion[3] = temp_quat[3];
	}

	float _q0 = quaternion[0];
	float _q1 = quaternion[1];
	float _q2 = quaternion[2];
	float _q3 = quaternion[3];

	*roll = RAD_TO_DEG * -atan2f(2.0f * (_q0*_q3 + _q1*_q2), 1.0f - 2.0f * (_q2*_q2 + _q3*_q3));
	*yaw = RAD_TO_DEG * -asinf(2.0f * (_q0*_q2 - _q3*_q1));
	*pitch = RAD_TO_DEG * atan2f(2.0f * (_q0*_q1 + _q2*_q3), 1.0f - 2.0f * (_q1*_q1 + _q2*_q2));

//	if (c_pitch > 0.0f) {
//		*pitch = 180.0f - c_pitch;
//	} else if (c_pitch < 0.0f) {
//		*pitch = -180.0f - c_pitch;
//	} else {
//		*pitch = 0.0f;
//	}
}
