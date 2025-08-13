/*
 * esp32.c
 *
 *  Created on: Jul 24, 2025
 *      Author: aravs
 */

#include "esp32.h"

extern I2C_HandleTypeDef hi2c2;

uint8_t tx_buffers[2][ESP32_TX_BUFFER_SIZE];
uint8_t tx_active = 0;

uint8_t rx_buffer[ESP32_RX_BUFFER_SIZE];

esp32_instruction instruction = {0};
extern rocket_data r_data;


uint32_t esp32_tx_counter = 0;

extern void process_esp32_instruction(esp32_instruction* instruction);

void initialize_esp32() {
	memset(tx_buffers, 0, sizeof(tx_buffers));
	HAL_I2C_EnableListen_IT(&hi2c2);
}

extern void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2) {
		HAL_I2C_EnableListen_IT(hi2c);
	}
}

extern void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	if (hi2c->Instance == I2C2) {
		if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, rx_buffer, ESP32_RX_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME);
		} else {
			uint8_t inactive = !tx_active;

			// fill inactive buffer
			memcpy(tx_buffers[inactive], &r_data, sizeof(rocket_data));

			tx_active = inactive;

			HAL_I2C_Slave_Seq_Transmit_IT(hi2c, tx_buffers[tx_active], ESP32_TX_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME);
		}
	}
}

extern void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2) {
		instruction.type = rx_buffer[0];
		instruction.payload_size = rx_buffer[1];
		memcpy(instruction.payload, rx_buffer + 2, instruction.payload_size);

		process_esp32_instruction(&instruction);

		HAL_I2C_EnableListen_IT(hi2c);
	}
}

extern void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2) {
		esp32_tx_counter++;
		HAL_I2C_EnableListen_IT(hi2c);
	}
}

extern void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2) {
		HAL_I2C_DeInit(hi2c);
		HAL_I2C_Init(hi2c);
		HAL_I2C_EnableListen_IT(hi2c);
	}
}
