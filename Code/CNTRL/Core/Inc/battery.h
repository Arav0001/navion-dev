/*
 * battery.h
 *
 *  Created on: Jul 22, 2025
 *      Author: aravs
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "main.h"

void battery_adc_start();
float battery_calculate_voltage();
void battery_update_voltage();

extern void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

#endif /* INC_BATTERY_H_ */
