#ifndef STM32_I2C_H
#define STM32_I2C_H

#include <Wire.h>

#include "util.h"

#define STM32_I2C_ADDRESS 0x28

#define ESP32_MAX_PAYLOAD_SIZE 32

typedef enum {
	ESP32_LAUNCH,
	ESP32_TVC_SERVO_X_POS,
	ESP32_TVC_SERVO_Y_POS,
	ESP32_TVC_DEFLECTION_POS
} esp32_instruction_type;

typedef struct {
	uint8_t type;
	uint8_t payload_size;
	uint8_t payload[ESP32_MAX_PAYLOAD_SIZE];
} __attribute__((packed)) esp32_instruction;

void initializeI2C();

void sendInstructionPacket(esp32_instruction* instruction);
bool requestStatePacket();

#endif // STM32_I2C_H
