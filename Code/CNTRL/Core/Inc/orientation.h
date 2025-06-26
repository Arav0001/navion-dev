/*
 * orientation.h
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#ifndef INC_ORIENTATION_H_
#define INC_ORIENTATION_H_

#include "main.h"

#include "stddef.h"
#include "motion_fx.h"

#define RAD_TO_DEG 57.2957795f
#define CONSTANT_g 9.8067f

#define MOTION_FX_STATE_SIZE (size_t)(2450)

HAL_StatusTypeDef initialize_MFX_orientation(MFX_engine_fusionType_t engine_dof);

void calculate_MFX_orientation(float* quaternion);

float get_MFX_orientation_freq();

#endif /* INC_ORIENTATION_H_ */
