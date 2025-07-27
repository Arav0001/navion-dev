/*
 * esp32.h
 *
 *  Created on: Jul 24, 2025
 *      Author: aravs
 */

#ifndef INC_ESP32_H_
#define INC_ESP32_H_

#include "main.h"

#include "util.h"

#define ESP32_MAX_PAYLOAD_SIZE 32

#define ESP32_TX_BUFFER_SIZE sizeof(rocket_data)
#define ESP32_RX_BUFFER_SIZE sizeof(esp32_instruction)

typedef enum {
	ESP32_LAUNCH = 0
} esp32_instruction_type;

typedef struct {
	uint8_t type;
	uint8_t payload_size;
	uint8_t payload[ESP32_MAX_PAYLOAD_SIZE];
} __attribute__((packed)) esp32_instruction;

void initialize_esp32();

extern void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
extern void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
extern void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
extern void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
extern void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);

#endif /* INC_ESP32_H_ */
