/*
 * bno055.c
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#include "Drivers/bno055.h"

#define BNO055_CHIP_ID 0xA0
#define BNO055_ACC_ID 0xFB
#define BNO055_GYR_ID 0x0F
#define BNO055_MAG_ID 0x32

#define BNO055_RESET_GPIO_Port IMU_RESET_GPIO_Port
#define BNO055_RESET_Pin IMU_RESET_Pin

#define BNO055_LINEAR_PASS_ALPHA 0.1

HAL_StatusTypeDef BNO055_WriteRegister(bno055_handle *bno055, uint8_t reg, uint8_t value) {
    uint8_t instructions_buf[2] = {
        reg,
        value
    };

    return HAL_I2C_Master_Transmit(bno055->i2c_handle, bno055->address << 1, instructions_buf, 2, 100);
}

HAL_StatusTypeDef BNO055_ReadRegister(bno055_handle *bno055, uint8_t reg, uint8_t* data, uint8_t size) {
    HAL_StatusTypeDef status_tx = HAL_I2C_Master_Transmit(bno055->i2c_handle, bno055->address << 1, &reg, 1, 100);
    HAL_StatusTypeDef status_rx = HAL_I2C_Master_Receive(bno055->i2c_handle, bno055->address << 1, data, size, 100);

    if (status_tx == HAL_ERROR || status_rx == HAL_ERROR) return HAL_ERROR;
    if (status_tx == HAL_BUSY || status_rx == HAL_BUSY) return HAL_BUSY;
    if (status_tx == HAL_TIMEOUT || status_rx == HAL_TIMEOUT) return HAL_TIMEOUT;

    return HAL_OK;
}

uint8_t BNO055_ReadChipID(bno055_handle *bno055) {
    BNO055_SetPage(bno055, 0);

    uint8_t chip_id;
    BNO055_ReadRegister(bno055, BNO055_CHIP_ID_ADDR, &chip_id, 1);
    return chip_id;
}

uint8_t BNO055_ReadAccID(bno055_handle *bno055) {
    BNO055_SetPage(bno055, 0);

    uint8_t acc_id;
    BNO055_ReadRegister(bno055, BNO055_ACC_ID_ADDR, &acc_id, 1);
    return acc_id;
}

uint8_t BNO055_ReadGyrID(bno055_handle *bno055) {
    BNO055_SetPage(bno055, 0);

    uint8_t gyr_id;
    BNO055_ReadRegister(bno055, BNO055_GYR_ID_ADDR, &gyr_id, 1);
    return gyr_id;
}

uint8_t BNO055_ReadMagID(bno055_handle *bno055) {
    BNO055_SetPage(bno055, 0);

    uint8_t mag_id;
    BNO055_ReadRegister(bno055, BNO055_MAG_ID_ADDR, &mag_id, 1);
    return mag_id;
}

void BNO055_SetPage(bno055_handle *bno055, uint8_t page) {
    BNO055_WriteRegister(bno055, BNO055_PAGE_ID_ADDR, page);
}

void BNO055_SetPwrMode(bno055_handle *bno055, bno055_pwr_mode pwr_mode) {
    BNO055_WriteRegister(bno055, BNO055_PWR_MODE_ADDR, pwr_mode);
}

void BNO055_SetOprMode(bno055_handle *bno055, bno055_opr_mode opr_mode) {
    BNO055_WriteRegister(bno055, BNO055_OPR_MODE_ADDR, opr_mode);
}

void BNO055_SetUnits(bno055_handle *bno055, bno055_units acc, bno055_units gyr, bno055_units eul, bno055_units temp) {
    BNO055_WriteRegister(bno055, BNO055_UNIT_SEL_ADDR, acc + gyr + eul + temp);
}

void BNO055_ConfigureAcc(bno055_handle *bno055, acc_g_range g_range, acc_bandwidth bandwidth, acc_opr_mode opr_mode) {
    BNO055_WriteRegister(bno055, BNO055_ACC_CONFIG_ADDR, g_range + bandwidth + opr_mode);
}

void BNO055_ConfigureGyr(bno055_handle *bno055, gyr_range range, gyr_bandwidth bandwidth, gyr_opr_mode opr_mode) {
    BNO055_WriteRegister(bno055, BNO055_GYR_CONFIG_0_ADDR, range + bandwidth);
    BNO055_WriteRegister(bno055, BNO055_GYR_CONFIG_1_ADDR, opr_mode);
}

void BNO055_ConfigureMag(bno055_handle *bno055, mag_data_rate data_rate, mag_opr_mode opr_mode, mag_pwr_mode pwr_mode) {
    BNO055_WriteRegister(bno055, BNO055_MAG_CONFIG_ADDR, data_rate + opr_mode + pwr_mode);
}

void BNO055_Configure(bno055_handle *bno055, bno055_config* config) {
	BNO055_ConfigureAcc(bno055, config->acc_config.g_range, config->acc_config.bandwidth, config->acc_config.opr_mode);
	BNO055_ConfigureGyr(bno055, config->gyr_config.range, config->gyr_config.bandwidth, config->gyr_config.opr_mode);
	BNO055_ConfigureMag(bno055, config->mag_config.data_rate, config->mag_config.opr_mode, config->mag_config.pwr_mode);

	BNO055_SetUnits(bno055, config->units.acc, config->units.gyr, config->units.eul, config->units.temp);
}

HAL_StatusTypeDef BNO055_Init(bno055_handle *bno055, bno055_config* config) {
	HAL_GPIO_WritePin(BNO055_RESET_GPIO_Port, BNO055_RESET_Pin, GPIO_PIN_SET);

	HAL_Delay(700);

	uint8_t chip_id = BNO055_ReadChipID(bno055);

    if (chip_id != BNO055_CHIP_ID) return HAL_ERROR;

    BNO055_SetOprMode(bno055, BNO055_OPR_CONFIG);

    // TODO: select using external oscillator

    HAL_Delay(50);

    BNO055_SetPwrMode(bno055, config->pwr_mode);
    HAL_Delay(50);

    BNO055_SetPage(bno055, 1);
    HAL_Delay(50);

    BNO055_Configure(bno055, config);
    HAL_Delay(50);

	BNO055_SetPage(bno055, 0);
	HAL_Delay(50);

    BNO055_SetOprMode(bno055, config->opr_mode);
    HAL_Delay(50);

    return HAL_OK;
}

void BNO055_ReadAcc(bno055_handle *bno055) {
    uint8_t buffer[6];

    BNO055_ReadRegister(bno055, BNO055_ACC_DATA_ADDR, buffer, 6);

    uint16_t x = (uint16_t)((buffer[1] << 8) | buffer[0]);
    uint16_t y = (uint16_t)((buffer[3] << 8) | buffer[2]);
    uint16_t z = (uint16_t)((buffer[5] << 8) | buffer[4]);

    bno055->data->ax = x;
    bno055->data->ay = y;
    bno055->data->az = z;
}

void BNO055_ReadGyr(bno055_handle *bno055) {
    uint8_t buffer[6];

    BNO055_ReadRegister(bno055, BNO055_GYR_DATA_ADDR, buffer, 6);

    uint16_t x = (uint16_t)((buffer[1] << 8) | buffer[0]);
    uint16_t y = (uint16_t)((buffer[3] << 8) | buffer[2]);
    uint16_t z = (uint16_t)((buffer[5] << 8) | buffer[4]);

    bno055->data->gx = x;
    bno055->data->gy = y;
    bno055->data->gz = z;
}

void BNO055_ReadMag(bno055_handle *bno055) {
    uint8_t buffer[6];

    BNO055_ReadRegister(bno055, BNO055_MAG_DATA_ADDR, buffer, 6);

    uint16_t x = (uint16_t)((buffer[1] << 8) | buffer[0]);
    uint16_t y = (uint16_t)((buffer[3] << 8) | buffer[2]);
    uint16_t z = (uint16_t)((buffer[5] << 8) | buffer[4]);

    bno055->data->mx = x;
    bno055->data->my = y;
    bno055->data->mz = z;
}
