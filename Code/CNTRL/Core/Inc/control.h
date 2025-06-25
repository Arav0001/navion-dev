/*
 * control.h
 *
 *  Created on: Jun 20, 2025
 *      Author: aravs
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "util.h"
#include "linmath.h"

typedef struct {
	vec3 a;
	vec3 g;
	vec3 m;

	quat orientation;

	float altitude;
	float temperature;

	float tvc_angle;

	float thrust;
} flight_state;

#endif /* INC_CONTROL_H_ */
