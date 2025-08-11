/*
 * control.h
 *
 *  Created on: Jun 20, 2025
 *      Author: aravs
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "main.h"

typedef struct {
	float Kp, Ki, Kd, Ka;

	const float OUT_MAX;
	const float OUT_MIN;

	uint32_t pt;
	float pe;

	float p;
	float i;
	float d;

	float cfe;
	float pfe;

	float output;
} PID;

void PID_init(PID* pid);
void PID_compute(PID* pid, float target, float current);

typedef struct {
	uint32_t time;

	float x[2];
	float P[2][2];
	float Q[2][2];
	float R;
} Kalman_2D;

void Kalman_2D_altitude_initialize(Kalman_2D* kalman, float Q[2][2], float R);
void Kalman_2D_altitude_predict(Kalman_2D* kalman, float acceleration);
void Kalman_2D_altitude_update(Kalman_2D* kalman, float altitude);

#endif /* INC_CONTROL_H_ */
