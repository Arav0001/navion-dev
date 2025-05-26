/*
 * bmp390.c
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#include "Drivers/bmp390.h"
#include <stdio.h>
#include <string.h>

#define BMP390_CSB_GPIO_PORT SPI1_CSB_GPIO_Port
#define BMP390_CSB_PIN SPI1_CSB_Pin

#define BMP390_INT_GPIO_PORT BMP_INT_GPIO_Port
#define BMP390_INT_PIN BMP_INT_Pin

uint8_t global_tx_buf[512];
uint8_t global_rx_buf[512];

void BMP390_SPI_Write(bmp390_handle *bmp390, uint8_t* data, uint16_t size) {
    HAL_SPI_Transmit(bmp390->spi_handle, data, size, 1000);
}

void BMP390_SPI_Read(bmp390_handle *bmp390, uint8_t* data, uint16_t size) {
    HAL_SPI_Receive(bmp390->spi_handle, data, size, 1000);
}

void BMP390_Select(bmp390_handle *bmp390) {
    HAL_GPIO_WritePin(BMP390_CSB_GPIO_PORT, BMP390_CSB_PIN, GPIO_PIN_RESET);
}

void BMP390_Deselect(bmp390_handle *bmp390) {
    HAL_GPIO_WritePin(BMP390_CSB_GPIO_PORT, BMP390_CSB_PIN, GPIO_PIN_SET);
}

void BMP390_SPI_WriteRegister(bmp390_handle *bmp390, uint8_t reg, uint8_t byte) {
    uint8_t instructions_buf[2] = {
        reg,
        byte
    };

    BMP390_Select(bmp390);
    BMP390_SPI_Write(bmp390, instructions_buf, 2);
    BMP390_Deselect(bmp390);
}

void BMP390_SPI_ReadRegister(bmp390_handle *bmp390, uint8_t reg, uint8_t* data, uint16_t size) {
    global_tx_buf[0] = reg | 0x80;

	BMP390_Select(bmp390);

	BMP390_SPI_Write(bmp390, global_tx_buf, 1);
    BMP390_SPI_Read(bmp390, global_rx_buf, size + 1);

	while (bmp390->spi_handle->State == HAL_SPI_STATE_BUSY);

	BMP390_Deselect(bmp390);

	memcpy(data, global_rx_buf + 1, size);
}

uint8_t BMP390_GetChipID(bmp390_handle *bmp390) {
    uint8_t chipID;
    BMP390_SPI_ReadRegister(bmp390, BMP390_CHIP_ID, &chipID, 1);
    return chipID;
}

uint8_t BMP390_GetRevID(bmp390_handle *bmp390) {
    uint8_t revID;
    BMP390_SPI_ReadRegister(bmp390, BMP390_REV_ID, &revID, 1);
    return revID;
}

void BMP390_GetError(bmp390_handle *bmp390) {
	uint8_t error;
	BMP390_SPI_ReadRegister(bmp390, BMP390_ERR_REG, &error, 1);

	bmp390->error->fatal         = (error >> 0) & 0x01;
	bmp390->error->command       = (error >> 1) & 0x01;
	bmp390->error->configuration = (error >> 2) & 0x01;
}

void BMP390_GetStatus(bmp390_handle *bmp390) {
	uint8_t status;
	BMP390_SPI_ReadRegister(bmp390, BMP390_STATUS, &status, 1);

	bmp390->status->command_in_progress     = (status >> 4) & 0x01;
	bmp390->status->pressure_data_ready     = (status >> 5) & 0x01;
	bmp390->status->temperature_data_ready  = (status >> 6) & 0x01;
}

uint8_t BMP390_GetInterruptStatus(bmp390_handle *bmp390) {
	uint8_t interrupt_status;
	BMP390_SPI_ReadRegister(bmp390, BMP390_INT_STATUS, &interrupt_status, 1);

	return (interrupt_status >> 3) & 0x01;
}

void BMP390_Init(bmp390_handle *bmp390) {
	// BMP390_SPI_WriteRegister(bmp390, BMP390_CMD, 0xB6);
	// HAL_Delay(10);

	BMP390_SPI_WriteRegister(bmp390, BMP390_PWR_CTRL, 0x33);

	// BMP390_SPI_WriteRegister(bmp390, BMP390_OSR, 0x0A);

	// BMP390_SPI_WriteRegister(bmp390, BMP390_ODR, 0x03);

	// BMP390_SPI_WriteRegister(bmp390, BMP390_CONFIG, 0x02);

	BMP390_SPI_WriteRegister(bmp390, BMP390_INT_CTRL, 0x42);

	// uint8_t interrupt_status = BMP390_GetInterruptStatus(bmp390);

	HAL_Delay(50);
}

void BMP390_Read(bmp390_handle *bmp390) {
    uint8_t data[6];

    BMP390_SPI_ReadRegister(bmp390, BMP390_DATA_0, data, 6);

    uint32_t pressure = (data[2] << 16) | (data[1] << 8) | data[0];
    uint32_t temperature = (data[5] << 16) | (data[4] << 8) | data[3];

    bmp390->data->pressure = pressure;
    bmp390->data->temperature = temperature;
}
