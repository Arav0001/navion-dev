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

#endif /* INC_CONTROL_H_ */
