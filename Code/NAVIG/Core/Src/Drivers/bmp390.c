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

void BMP390_ParseCalibData(bmp390_handle *bmp390, uint8_t* data) {
	bmp390->calib->par_t1 = CONCAT_BYTES(data[1], data[0]);
	bmp390->calib->par_t2 = CONCAT_BYTES(data[3], data[2]);
	bmp390->calib->par_t3 = (int8_t)data[4];
	bmp390->calib->par_p1 = (int16_t)CONCAT_BYTES(data[6], data[5]);
	bmp390->calib->par_p2 = (int16_t)CONCAT_BYTES(data[8], data[7]);
	bmp390->calib->par_p3 = (int8_t)data[9];
	bmp390->calib->par_p4 = (int8_t)data[10];
	bmp390->calib->par_p5 = CONCAT_BYTES(data[12], data[11]);
	bmp390->calib->par_p6 = CONCAT_BYTES(data[14], data[13]);
	bmp390->calib->par_p7 = (int8_t)data[15];
	bmp390->calib->par_p8 = (int8_t)data[16];
	bmp390->calib->par_p9 = (int16_t)CONCAT_BYTES(data[18], data[17]);
	bmp390->calib->par_p10 = (int8_t)data[19];
	bmp390->calib->par_p11 = (int8_t)data[20];
}

void BMP390_ReadCalibData(bmp390_handle *bmp390) {
	uint8_t calib_data_buffer[BMP390_CALIB_DATA_LEN] = {0};

	BMP390_SPI_ReadRegister(bmp390, BMP390_CALIB, calib_data_buffer, BMP390_CALIB_DATA_LEN);

	BMP390_ParseCalibData(bmp390, calib_data_buffer);
}

int64_t BMP390_CompensateTemperature(bmp390_handle *bmp390) {
	int64_t partial_data1;
	int64_t partial_data2;
	int64_t partial_data3;
	int64_t partial_data4;
	int64_t partial_data5;
	int64_t partial_data6;
	int64_t comp_temp;

	partial_data1 = ((int64_t)bmp390->uncomp_data->temperature - (256 * bmp390->calib->par_t1));
	partial_data2 = bmp390->calib->par_t2 * partial_data1;
	partial_data3 = (partial_data1 * partial_data1);
	partial_data4 = (int64_t)partial_data3 * bmp390->calib->par_t3;
	partial_data5 = ((int64_t)(partial_data2 * 262144) + partial_data4);
	partial_data6 = partial_data5 / 4294967296;

	// store t_lin for pressure calculation
	bmp390->calib->t_lin = partial_data6;
	comp_temp = (int64_t)((partial_data6 * 25) / 16384);

	return comp_temp;
}

uint64_t BMP390_CompensatePressure(bmp390_handle *bmp390) {
	int64_t partial_data1;
	int64_t partial_data2;
	int64_t partial_data3;
	int64_t partial_data4;
	int64_t partial_data5;
	int64_t partial_data6;
	int64_t offset;
	int64_t sensitivity;
	uint64_t comp_press;

	partial_data1 = bmp390->calib->t_lin * bmp390->calib->t_lin;
	partial_data2 = partial_data1 / 64;
	partial_data3 = (partial_data2 * bmp390->calib->t_lin) / 256;
	partial_data4 = (bmp390->calib->par_p8 * partial_data3) / 32;
	partial_data5 = (bmp390->calib->par_p7 * partial_data1) * 16;
	partial_data6 = (bmp390->calib->par_p6 * bmp390->calib->t_lin) * 4194304;
	offset = (bmp390->calib->par_p5 * 140737488355328) + partial_data4 + partial_data5 + partial_data6;
	partial_data2 = (bmp390->calib->par_p4 * partial_data3) / 32;
	partial_data4 = (bmp390->calib->par_p3 * partial_data1) * 4;
	partial_data5 = (bmp390->calib->par_p2 - 16384) * bmp390->calib->t_lin * 2097152;
	sensitivity = ((bmp390->calib->par_p1 - 16384) * 70368744177664) + partial_data2 + partial_data4 + partial_data5;
	partial_data1 = (sensitivity / 16777216) * bmp390->uncomp_data->pressure;
	partial_data2 = bmp390->calib->par_p10 * bmp390->calib->t_lin;
	partial_data3 = partial_data2 + (65536 * bmp390->calib->par_p9);
	partial_data4 = (partial_data3 * bmp390->uncomp_data->pressure) / 8192;

	/* dividing by 10 followed by multiplying by 10
	 * To avoid overflow caused by (bmp390->uncomp_data->pressure * partial_data4)
	 */
	partial_data5 = (bmp390->uncomp_data->pressure * (partial_data4 / 10)) / 512;
	partial_data5 = partial_data5 * 10;
	partial_data6 = (int64_t)((uint64_t)bmp390->uncomp_data->pressure * (uint64_t)bmp390->uncomp_data->pressure);
	partial_data2 = (bmp390->calib->par_p11 * partial_data6) / 65536;
	partial_data3 = (partial_data2 * bmp390->uncomp_data->pressure) / 128;
	partial_data4 = (offset / 4) + partial_data1 + partial_data5 + partial_data3;
	comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776);

	return comp_press;
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

	BMP390_ReadCalibData(bmp390);

	HAL_Delay(50);
}

void BMP390_Read(bmp390_handle *bmp390) {
    uint8_t data[6];

    BMP390_SPI_ReadRegister(bmp390, BMP390_DATA_0, data, 6);

    uint32_t pressure = (data[2] << 16) | (data[1] << 8) | data[0];
    uint32_t temperature = (data[5] << 16) | (data[4] << 8) | data[3];

    bmp390->uncomp_data->pressure = pressure;
    bmp390->uncomp_data->temperature = temperature;

    int64_t comp_temperature = BMP390_CompensateTemperature(bmp390);
    uint64_t comp_pressure = BMP390_CompensatePressure(bmp390);

    bmp390->data->temperature = comp_temperature;
    bmp390->data->pressure = comp_pressure;
}
