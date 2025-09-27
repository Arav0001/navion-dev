/*
 * tvc.c
 *
 *  Created on: Jun 21, 2025
 *      Author: aravs
 */

#include "tvc.h"

#include "util.h"

uint8_t locked = 1;

void tvc_init(tvc_mount* tvc) {
	tvc->x.config.INIT_ANGLE = CONFIG_TVC_X_INIT_ANGLE;
	tvc->y.config.INIT_ANGLE = CONFIG_TVC_Y_INIT_ANGLE;

	servo_start(&tvc->x);
	servo_start(&tvc->y);
}

void tvc_start(tvc_mount* tvc) {
	PID_init(&tvc->pid_x);
	PID_init(&tvc->pid_y);
}

void tvc_stop(tvc_mount* tvc) {
	servo_stop(&tvc->x);
	servo_stop(&tvc->y);
}

static float clamp_angle(float angle, float max_deviation) {
	if (angle >  max_deviation) {
		return max_deviation;
	} else if (angle < -max_deviation) {
		return -max_deviation;
	} else {
		return angle;
	}
}

void tvc_set_angle(tvc_mount* tvc, float angle, tvc_servo_type servo) {
	if (servo == TVC_SERVO_X) {
		float X = angle * CONFIG_TVC_X_SCALE_FAC;
		X = CONFIG_TVC_X_INIT_ANGLE + clamp_angle(X, CONFIG_TVC_X_MAX_D_ANGLE);

		servo_set_angle(&tvc->x, X);
	} else if (servo == TVC_SERVO_Y) {
		float Y = angle * CONFIG_TVC_Y_SCALE_FAC;
		Y = CONFIG_TVC_Y_INIT_ANGLE + clamp_angle(Y, CONFIG_TVC_Y_MAX_D_ANGLE);

		servo_set_angle(&tvc->y, Y);
	}
}

void tvc_set_angles_f(tvc_mount* tvc, float x, float y) {
	tvc_set_angle(tvc, x, TVC_SERVO_X);
	tvc_set_angle(tvc, y, TVC_SERVO_Y);

	tvc->ax = x;
	tvc->ay = y;
}

void tvc_lock() {
	locked = 1;
}

void tvc_unlock() {
	locked = 0;
}

void tvc_update(tvc_mount* tvc, float target_pitch, float target_yaw, float current_pitch, float current_yaw) {
	if (current_pitch >= CONFIG_TVC_MAX_TILT || current_pitch <= -CONFIG_TVC_MAX_TILT || current_yaw >= CONFIG_TVC_MAX_TILT || current_yaw <= -CONFIG_TVC_MAX_TILT) {
		locked = 1;
	}

	if (locked) {
		tvc_set_angles_f(tvc, 0.0f, 0.0);
	} else {
		PID_compute(&tvc->pid_x, target_pitch, current_pitch);
		PID_compute(&tvc->pid_y, target_yaw, current_yaw);

		tvc->ax = tvc->pid_x.output;
		tvc->ay = tvc->pid_y.output;

		tvc_set_angles_f(tvc, tvc->pid_x.output, tvc->pid_y.output);
	}
}
