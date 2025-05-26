/*
 * neom9n.h
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_NEOM9N_H_
#define INC_DRIVERS_NEOM9N_H_

#include "main.h"

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t z;
} neom9n_data;

typedef struct {
	I2C_HandleTypeDef i2c_handle;
	uint8_t address;
	neom9n_data data;
} neom9n_handle;

#endif /* INC_DRIVERS_NEOM9N_H_ */
