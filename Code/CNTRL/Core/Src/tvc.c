/*
 * tvc.c
 *
 *  Created on: Jun 21, 2025
 *      Author: aravs
 */

#include "tvc.h"

void tvc_start(tvc_mount* tvc) {
	tvc->x.config.INIT_ANGLE = CONFIG_TVC_X_INIT_ANGLE;
	tvc->y.config.INIT_ANGLE = CONFIG_TVC_Y_INIT_ANGLE;

	servo_start(&tvc->x);
	servo_start(&tvc->y);
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

void tvc_set_angles_vec(tvc_mount* tvc, vec2 angles) {
	tvc_set_angle(tvc, angles[0], TVC_SERVO_X);
	tvc_set_angle(tvc, angles[1], TVC_SERVO_Y);
}

void tvc_set_angles_f(tvc_mount* tvc, float x, float y) {
	tvc_set_angle(tvc, x, TVC_SERVO_X);
	tvc_set_angle(tvc, y, TVC_SERVO_Y);
}
