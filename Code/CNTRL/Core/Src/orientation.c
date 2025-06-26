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

static uint8_t mfxstate[MOTION_FX_STATE_SIZE];
MFX_knobs_t knobs;

float last_time;
float current_time;
float dt;

float orientation_freq;

HAL_StatusTypeDef initialize_MFX_orientation(MFX_engine_fusionType_t engine_dof) {
	if (MOTION_FX_STATE_SIZE < MotionFX_GetStateSize()) {
		return HAL_ERROR;
	}

	MotionFX_initialize((MFXState_t*)mfxstate);

	MotionFX_getKnobs(mfxstate, &knobs);

	// modify knobs here if needed

	MotionFX_setKnobs(mfxstate, &knobs);

	MotionFX_enable_6X(mfxstate, MFX_ENGINE_DISABLE);
	MotionFX_enable_9X(mfxstate, MFX_ENGINE_DISABLE);

	if (engine_dof == MFX_ENGINE_6X) {
		MotionFX_enable_6X(mfxstate, MFX_ENGINE_ENABLE);
		return HAL_OK;
	} else if (engine_dof == MFX_ENGINE_9X) {
		MotionFX_enable_9X(mfxstate, MFX_ENGINE_ENABLE);
		return HAL_OK;
	} else {
		return HAL_ERROR;
	}
}

void calculate_MFX_orientation(float* quaternion) {
	MFX_input_t data_in;
	MFX_output_t data_out;

	current_time = HAL_GetTick();

	/* Get acceleration X/Y/Z in g */
	data_in.acc[0] = data.ax / CONSTANT_g;
	data_in.acc[1] = data.ay / CONSTANT_g;
	data_in.acc[2] = data.az / CONSTANT_g;

	/* Get angular rate X/Y/Z in dps */
	data_in.gyro[0] = data.gx * RAD_TO_DEG;
	data_in.gyro[1] = data.gy * RAD_TO_DEG;
	data_in.gyro[2] = data.gz * RAD_TO_DEG;

	/* Get magnetic field X/Y/Z in uT/50 */
	data_in.mag[0] = data.mx * 50.0f;
	data_in.mag[1] = data.my * 50.0f;
	data_in.mag[2] = data.mz * 50.0f;

	/* Calculate elapsed time from last accesing this function in seconds */
	dt = (current_time - last_time) / 1000.0f;
	last_time = current_time;

	orientation_freq = 1.0f / dt;

	/* Run Sensor Fusion algorithm */
	MotionFX_propagate(mfxstate, &data_out, &data_in, &dt);
	MotionFX_update(mfxstate, &data_out, &data_in, &dt, NULL);

	memcpy(quaternion, data_out.quaternion, 4 * sizeof(float));
}

float get_MFX_orientation_freq() {
	return orientation_freq;
}
