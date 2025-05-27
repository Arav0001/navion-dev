/*
 * i2c_slave.c
 *
 *  Created on: Mar 20, 2025
 *      Author: aravs
 */

#include "i2c_slave.h"

#include "usbd_cdc_if.h"

extern i2c_slave i2c1_slave;

void I2C_Slave_Init(i2c_slave *slave, I2C_HandleTypeDef *hi2c) {
	slave->i2c_handle = hi2c;
	slave->rx_index = 0;
	slave->tx_index = 0;
	slave->tx_length = 0;
	slave->is_receiving = 0;
	slave->is_transmitting = 0;

	if (HAL_I2C_EnableListen_IT(slave->i2c_handle) != HAL_OK) {
		while (1) {
			HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
			HAL_Delay(500);
		}
	}
}

extern void HAL_I2C_ListenCpltCallback (I2C_HandleTypeDef *hi2c) {
	HAL_I2C_EnableListen_IT(hi2c);
}

// TODO: fix for any size data can be received
extern void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	i2c_slave *slave = &i2c1_slave;

	if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
		// Master wants to write
		slave->is_receiving = 1;
		slave->rx_index = 0;
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, slave->rx_buffer, PACKET_SIZE, I2C_FIRST_AND_LAST_FRAME);
	} else {
		// Master wants to read
		slave->is_transmitting = 1;
		slave->tx_index = 0;
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, slave->tx_buffer, slave->tx_length, I2C_FIRST_AND_LAST_FRAME);
	}
}

extern void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	i2c_slave *slave = &i2c1_slave;

	slave->is_receiving = 0;

	// Send data to USB CDC
	// CDC_Transmit_FS(slave->rx_buffer, RX_SIZE);

	HAL_I2C_EnableListen_IT(hi2c);
}

extern void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	i2c_slave *slave = &i2c1_slave;

    slave->is_transmitting = 0;
    HAL_I2C_EnableListen_IT(hi2c);
}

extern void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_EnableListen_IT(hi2c);
}
