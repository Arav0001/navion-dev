#include "stm32_i2c.h"

uint8_t tx_buffer[sizeof(esp32_instruction)];

void initializeI2C() {
    Wire.begin(SDA, SCL, 100000);
    delay(500);
}

void sendInstructionPacket(esp32_instruction* instruction) {
    memcpy(tx_buffer, instruction, sizeof(esp32_instruction));

    Wire.beginTransmission(STM32_I2C_ADDRESS);
    Wire.write(tx_buffer, sizeof(esp32_instruction));
    Wire.endTransmission();
}

void requestStatePacket() {
    Wire.requestFrom(STM32_I2C_ADDRESS, sizeof(rocket_data));
}