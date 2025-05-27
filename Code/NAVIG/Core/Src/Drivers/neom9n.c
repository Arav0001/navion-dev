/*
 * neom9n.c
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#include "Drivers/neom9n.h"

#include "main.h"

HAL_StatusTypeDef NEOM9N_ReadDataStream(neom9n_handle* neom9n, uint8_t* data, uint8_t size) {
	HAL_StatusTypeDef status_tx = HAL_I2C_Master_Transmit(neom9n->i2c_handle, neom9n->address << 1, NEOM9N_DATA_STREAM, 1, 100);
	HAL_StatusTypeDef status_rx = HAL_I2C_Master_Receive(neom9n->i2c_handle, neom9n->address << 1, data, size, 100);

	if (status_tx == HAL_ERROR || status_rx == HAL_ERROR) return HAL_ERROR;
	if (status_tx == HAL_BUSY || status_rx == HAL_BUSY) return HAL_BUSY;
	if (status_tx == HAL_TIMEOUT || status_rx == HAL_TIMEOUT) return HAL_TIMEOUT;

	return HAL_OK;
}
