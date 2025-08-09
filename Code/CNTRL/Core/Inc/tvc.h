/*
 * tvc.h
 *
 *  Created on: Jun 21, 2025
 *      Author: aravs
 */

#ifndef INC_TVC_H_
#define INC_TVC_H_

#include "main.h"
#include "config.h"

#include "Drivers/servo.h"

#include "control.h"

typedef enum {
	TVC_SERVO_X,
	TVC_SERVO_Y
} tvc_servo_type;

typedef struct {
	servo x;
	servo y;

	PID pid_x;
	PID pid_y;
} tvc_mount;

void tvc_init(tvc_mount* tvc);
void tvc_start(tvc_mount* tvc);
void tvc_stop(tvc_mount* tvc);

void tvc_set_angle(tvc_mount* tvc, float angle, tvc_servo_type servo);
void tvc_set_angles_f(tvc_mount* tvc, float x, float y);

void tvc_update(tvc_mount* tvc, float target_pitch, float target_yaw, float current_pitch, float current_yaw);

#endif /* INC_TVC_H_ */
