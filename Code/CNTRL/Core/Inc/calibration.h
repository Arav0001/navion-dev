/*
 * calibration.h
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#include "motion_ac.h"
#include "motion_gc.h"
#include "motion_mc.h"

void initialize_acc_calibration();
void run_acc_calibration();

char MotionAC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data);
char MotionAC_SaveCalInNVM(unsigned short int datasize, unsigned int *data);

void initialize_gyr_calibration();
void run_gyr_calibration();

void initialize_mag_calibration() ;
void run_mag_calibration();

char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data);
char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data);

#endif /* INC_CALIBRATION_H_ */
