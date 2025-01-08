#include "main.h"

void setStatusLED(uint8_t r, uint8_t g, uint8_t b) {
    TIM2->CCR2 = r * (65535 / 255);
    TIM2->CCR3 = g * (65535 / 255);
    TIM2->CCR4 = b * (65535 / 255);
}