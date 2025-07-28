/*
 * battery.c
 *
 *  Created on: Jul 22, 2025
 *      Author: aravs
 */

#include "battery.h"

#define ADC_BUF_LEN 2
#define ADC_RESOLUTION 4095.0f

#define BATTERY_DIVIDER_RATIO (1300.0f / 300.0f)

extern ADC_HandleTypeDef hadc1;

uint16_t adc_buf[ADC_BUF_LEN];  // IN0, VREFINT

float actual_vref = VREFINT_CAL_VREF;
float adc_in0 = 0.0f;

uint8_t battery_adc_ready = 0;

void battery_adc_start() {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN);
}

float battery_calculate_voltage() {
	uint16_t raw_in0   = adc_buf[0];
	uint16_t raw_vref  = adc_buf[1];
	uint16_t vref_cal  = *VREFINT_CAL_ADDR;

	actual_vref = VREFINT_CAL_VREF * 0.001f * ((float)vref_cal / (float)raw_vref);

	adc_in0 = ((float)raw_in0 / ADC_RESOLUTION) * actual_vref;

	return adc_in0 * BATTERY_DIVIDER_RATIO;
}

void battery_update_voltage(float* vbat) {
	if (battery_adc_ready) {
		battery_adc_ready = 0;
		*vbat = battery_calculate_voltage();
	}
}

extern void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC1) {
		battery_adc_ready = 1;
	}
}
