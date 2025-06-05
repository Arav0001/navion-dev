/*
 * neom9n.h
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_NEOM9N_H_
#define INC_DRIVERS_NEOM9N_H_

#include "main.h"

#define NEOM9N_NUM_BYTES_AVAILABLE_HIGH 0xFD
#define NEOM9N_NUM_BYTES_AVAILABLE_LOW 0xFE
#define NEOM9N_DATA_STREAM 0xFF

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t z;
} __attribute__((packed)) neom9n_data;

typedef struct {
	I2C_HandleTypeDef* i2c_handle;
	uint8_t address;
	neom9n_data* data;
} neom9n_handle;

HAL_StatusTypeDef NEOM9N_ReadDataStream(neom9n_handle* neom9n, uint8_t* data, uint8_t size);

#endif /* INC_DRIVERS_NEOM9N_H_ */
