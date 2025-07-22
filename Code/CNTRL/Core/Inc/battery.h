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

uint8_t battery_adc_is_ready();
void battery_adc_set_ready();
void battery_adc_set_not_ready();

#endif /* INC_BATTERY_H_ */
