/*
 * bmp390.h
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_BMP390_H_
#define INC_DRIVERS_BMP390_H_

#include "main.h"

#define BMP390_CHIP_ID          0x00
#define BMP390_REV_ID           0x01
#define BMP390_ERR_REG          0x02
#define BMP390_STATUS           0x03
#define BMP390_DATA_0           0x04
#define BMP390_DATA_1           0x05
#define BMP390_DATA_2           0x06
#define BMP390_DATA_3           0x07
#define BMP390_DATA_4           0x08
#define BMP390_DATA_5           0x09
#define BMP390_SENSORTIME_0     0x0C
#define BMP390_SENSORTIME_1     0x0D
#define BMP390_SENSORTIME_2     0x0E
#define BMP390_EVENT            0x10
#define BMP390_INT_STATUS       0x11
#define BMP390_FIFO_LENGTH_0    0x12
#define BMP390_FIFO_LENGTH_1    0x13
#define BMP390_FIFO_DATA        0x14
#define BMP390_FIFO_WTM_0       0x15
#define BMP390_FIFO_WTM_1       0x16
#define BMP390_FIFO_CONFIG_1    0x17
#define BMP390_FIFO_CONFIG_2    0x18
#define BMP390_INT_CTRL         0x19
#define BMP390_IF_CONF          0x1A
#define BMP390_PWR_CTRL         0x1B
#define BMP390_OSR              0x1C
#define BMP390_ODR              0x1D
#define BMP390_CONFIG           0x1F
#define BMP390_CALIB            0x31
#define BMP390_CMD              0x7E

#define BMP390_CALIB_DATA_LEN  21

#define CONCAT_BYTES(msb, lsb) (uint16_t)((msb << 8) | lsb)

typedef struct {
    uint64_t pressure;
    int64_t temperature;
} __attribute__((packed)) bmp390_data;

typedef struct {
	uint32_t pressure;
	uint32_t temperature;
} bmp390_uncomp_data;

typedef struct {
	uint8_t fatal          : 1;
	uint8_t command        : 1;
	uint8_t configuration  : 1;
	uint8_t reserved3_7	: 5;
} bmp390_error;

typedef struct {
	uint8_t reserved0_3 			: 4;
	uint8_t command_in_progress 	: 1;
	uint8_t pressure_data_ready 	: 1;
	uint8_t temperature_data_ready : 1;
	uint8_t reserved7 				: 1;
} bmp390_status;

typedef struct {
	uint16_t par_t1;
	uint16_t par_t2;
	int8_t par_t3;
	int16_t par_p1;
	int16_t par_p2;
	int8_t par_p3;
	int8_t par_p4;
	uint16_t par_p5;
	uint16_t par_p6;
	int8_t par_p7;
	int8_t par_p8;
	int16_t par_p9;
	int8_t par_p10;
	int8_t par_p11;
	int64_t t_lin;
} bmp390_calib;

typedef struct {
    SPI_HandleTypeDef* spi_handle;
    bmp390_data* data;
    bmp390_uncomp_data* uncomp_data;
    bmp390_calib* calib;

    bmp390_error* error;
    bmp390_status* status;
} bmp390_handle;

void BMP390_Select(bmp390_handle *bmp390);
void BMP390_Deselect(bmp390_handle *bmp390);

void BMP390_SPI_WriteRegister(bmp390_handle *bmp390, uint8_t reg, uint8_t byte);
void BMP390_SPI_ReadRegister(bmp390_handle *bmp390, uint8_t reg, uint8_t* data, uint16_t size);

uint8_t BMP390_GetChipID(bmp390_handle *bmp390);
uint8_t BMP390_GetRevID(bmp390_handle *bmp390);

void BMP390_GetError(bmp390_handle *bmp390);
void BMP390_GetStatus(bmp390_handle *bmp390);

void BMP390_Init(bmp390_handle *bmp390);
void BMP390_Read(bmp390_handle *bmp390);

#endif /* INC_DRIVERS_BMP390_H_ */
