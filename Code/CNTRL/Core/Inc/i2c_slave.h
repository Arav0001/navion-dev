/*
 * i2c_slave.h
 *
 *  Created on: Mar 20, 2025
 *      Author: aravs
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

#include "main.h"

#define PACKET_SIZE 48

#define RX_SIZE 256
#define TX_SIZE 256

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint8_t rx_buffer[RX_SIZE];
	uint16_t rx_index;
	uint8_t tx_buffer[TX_SIZE];
	uint16_t tx_index;
	uint16_t tx_length;
	uint8_t is_receiving;
	uint8_t is_transmitting;
} i2c_slave;

void I2C_Slave_Init(i2c_slave *slave, I2C_HandleTypeDef *hi2c);

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);

#endif /* INC_I2C_SLAVE_H_ */
