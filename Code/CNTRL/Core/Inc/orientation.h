/*
 * orientation.h
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#ifndef INC_ORIENTATION_H_
#define INC_ORIENTATION_H_

#include "main.h"

void initialize_orientation();

void calculate_orientation(float* quaternion);

float get_orientation_freq();

#endif /* INC_ORIENTATION_H_ */
