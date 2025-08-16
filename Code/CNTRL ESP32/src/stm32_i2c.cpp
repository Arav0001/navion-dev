#include "stm32_i2c.h"

uint8_t tx_buffer[sizeof(esp32_instruction)];
uint8_t rx_buffer[sizeof(rocket_data)];

extern rocket_data r_data;

unsigned long last_request_time = 0;
const unsigned long request_interval_ms = 50;

void initializeI2C() {
    Wire.begin(SDA, SCL, 100000);
    delay(500);
}

void sendInstructionPacket(esp32_instruction* instruction) {
    memset(tx_buffer, 0, sizeof(tx_buffer));
    memcpy(tx_buffer, instruction, sizeof(esp32_instruction));

    Wire.beginTransmission(STM32_I2C_ADDRESS);
    Wire.write(tx_buffer, sizeof(esp32_instruction));
    Wire.endTransmission();
}

bool requestStatePacket() {
    unsigned long now = millis();
    if (now - last_request_time < request_interval_ms) return false;

    if (Wire.requestFrom(STM32_I2C_ADDRESS, sizeof(rocket_data)) == sizeof(rocket_data)) {
        last_request_time = now;
        
        Wire.readBytes(rx_buffer, sizeof(rocket_data));
        memcpy(&r_data, rx_buffer, sizeof(rocket_data));

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        return true;
    }

    return false;
}