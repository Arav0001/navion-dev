/*
 * calibration.c
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#include "calibration.h"

#include "util.h"
#include "config.h"

extern sensor_data data;

#ifdef CALIBRATE
uint8_t acc_calibrated = 0;
int gyr_calibrated = 0;
#endif

#ifdef CALIBRATE_ACC
MAC_knobs_t acc_knobs;
MAC_output_t acc_cal;
#endif

#ifdef CALIBRATE_GYR
MGC_knobs_t gyr_knobs;
MGC_output_t gyr_cal;
float gyr_sample_freq = 20.0f;
#endif

#ifdef CALIBRATE_MAG
MMC_Output_t mag_cal;
#endif

void initialize_acc_calibration() {
#ifdef CALIBRATE_ACC
	MotionAC_Initialize(1);
	MotionAC_GetKnobs(&acc_knobs);
	acc_knobs.Run6PointCal = 1;
	acc_knobs.Sample_ms = 50;
	acc_knobs.MoveThresh_g = 0.20f;
	(void)MotionAC_SetKnobs(&acc_knobs);
#endif
	return;
}

void run_acc_calibration() {
#ifdef CALIBRATE_ACC
	if (!acc_calibrated) {
		MAC_input_t data_in;

		data_in.Acc[0] = data.ax / CONSTANT_g;
		data_in.Acc[1] = data.ay / CONSTANT_g;
		data_in.Acc[2] = data.az / CONSTANT_g;

		data_in.TimeStamp = HAL_GetTick();

		MotionAC_Update(&data_in, &acc_calibrated);
		MotionAC_GetCalParams(&acc_cal);
	}
#endif
	return;
}

char MotionAC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data) {
	return 1; /* FAILURE: Read from NVM not implemented. */
}

char MotionAC_SaveCalInNVM(unsigned short int datasize, unsigned int *data) {
	return 1; /* FAILURE: Write to NVM not implemented. */
}

void initialize_gyr_calibration() {
#ifdef CALIBRATE_GYR
	MotionGC_Initialize(MGC_MCU_STM32, &gyr_sample_freq);
	MotionGC_GetKnobs(&gyr_knobs);
	gyr_knobs.AccThr = 0.008f;
	gyr_knobs.GyroThr = 0.15f;
	MotionGC_SetKnobs(&gyr_knobs);
	MotionGC_SetFrequency(&gyr_sample_freq);
#endif
	return;
}

void run_gyr_calibration() {
#ifdef CALIBRATE_GYR
//	if (!gyr_calibrated) {
		MGC_input_t data_in;

		data_in.Acc[0] = data.ax / CONSTANT_g;
		data_in.Acc[1] = data.ay / CONSTANT_g;
		data_in.Acc[2] = data.az / CONSTANT_g;

		data_in.Gyro[0] = data.gx * RAD_TO_DEG;
		data_in.Gyro[1] = data.gy * RAD_TO_DEG;
		data_in.Gyro[2] = data.gz * RAD_TO_DEG;

		MotionGC_Update(&data_in, &gyr_cal, &gyr_calibrated);
		MotionGC_GetCalParams(&gyr_cal);
//	}
#endif
	return;
}

void initialize_mag_calibration() {
#ifdef CALIBRATE_MAG
	MotionMC_Initialize(50, 1);
#endif
	return;
}

void run_mag_calibration() {
#ifdef CALIBRATE_MAG
	MMC_Input_t data_in;

	data_in.Mag[0] = data.mx;
	data_in.Mag[1] = data.my;
	data_in.Mag[2] = data.mz;

	data_in.TimeStamp = HAL_GetTick();

	MotionMC_Update(&data_in);
	MotionMC_GetCalParams(&mag_cal);
#endif
	return;
}

char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data) {
	return 1; /* FAILURE: Read from NVM not implemented. */
}

char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data) {
	return 1; /* FAILURE: Write to NVM not implemented. */
}
