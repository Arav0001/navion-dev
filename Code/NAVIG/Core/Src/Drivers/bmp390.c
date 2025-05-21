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

void BMP390_SPI_Write(bmp390_handle *bmp390, uint8_t* data, uint16_t size) {
    HAL_SPI_Transmit(bmp390->spi_handle, data, size, 1000);
}

void BMP390_SPI_Read(bmp390_handle *bmp390, uint8_t* data, uint16_t size) {
    HAL_SPI_Receive(bmp390->spi_handle, data, size, 1000);
}

// TODO: fix selection depending on what the pullup/pulldown is
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
    uint8_t tx_buf[64];
    tx_buf[0] = reg | 0x80;
    uint8_t rx_buf[64];

	BMP390_Select(bmp390);
	//HAL_Delay(10);

	//HAL_SPI_TransmitReceive(bmp390->spi_handle, tx_buf, rx_buf, size+1, 1000);
    BMP390_SPI_Write(bmp390, tx_buf, 1);
    BMP390_SPI_Read(bmp390, rx_buf, size+1);
	while (bmp390->spi_handle->State == HAL_SPI_STATE_BUSY)
			;  // wait for xmission complete

	BMP390_Deselect(bmp390);

	memcpy(data, rx_buf+1, size);
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

void BMP390_Read(bmp390_handle *bmp390) {
    uint8_t data[6];
    uint8_t status_holder = 0, status = 0;

    while(status != 0x60)
    {
    	BMP390_SPI_ReadRegister(bmp390, BMP390_STATUS, &status_holder, 1);
    	status = (status_holder & 0x60);
    }

    BMP390_SPI_ReadRegister(bmp390, BMP390_DATA_0, data, 6);
    uint32_t pressure = (data[2] << 16) | (data[1] << 8) | data[0];
    uint32_t temperature = (data[5] << 16) | (data[4] << 8) | data[3];
}
