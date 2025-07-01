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

}

void calculate_orientation(float* quaternion) {

}

float get_orientation_freq() {
	return orientation_freq;
}
