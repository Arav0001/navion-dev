/*
 * bno055.h
 *
 *  Created on: Mar 22, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_BNO055_H_
#define INC_DRIVERS_BNO055_H_

#include "main.h"

// Sensor output data scaling constants
#define ACC_SCALER 0.01
#define GYR_SCALER 0.0625
#define MAG_SCALER 0.0625

/* PAGE 0*/
#define BNO055_CHIP_ID_ADDR          0x00
#define BNO055_ACC_ID_ADDR           0x01
#define BNO055_MAG_ID_ADDR           0x02
#define BNO055_GYR_ID_ADDR           0x03
#define BNO055_SW_REV_ID_LSB_ADDR    0x04
#define BNO055_SW_REV_ID_MSB_ADDR    0x05
#define BNO055_BL_REV_ID_ADDR        0x06
#define BNO055_PAGE_ID_ADDR          0x07
#define BNO055_ACC_DATA_X_LSB_ADDR   0x08
#define BNO055_ACC_DATA_X_MSB_ADDR   0x09
#define BNO055_ACC_DATA_Y_LSB_ADDR   0x0A
#define BNO055_ACC_DATA_Y_MSB_ADDR   0x0B
#define BNO055_ACC_DATA_Z_LSB_ADDR   0x0C
#define BNO055_ACC_DATA_Z_MSB_ADDR   0x0D
#define BNO055_ACC_DATA_ADDR         BNO055_ACC_DATA_X_LSB_ADDR
#define BNO055_MAG_DATA_X_LSB_ADDR   0x0E
#define BNO055_MAG_DATA_X_MSB_ADDR   0x0F
#define BNO055_MAG_DATA_Y_LSB_ADDR   0x10
#define BNO055_MAG_DATA_Y_MSB_ADDR   0x11
#define BNO055_MAG_DATA_Z_LSB_ADDR   0x12
#define BNO055_MAG_DATA_Z_MSB_ADDR   0x13
#define BNO055_MAG_DATA_ADDR         BNO055_MAG_DATA_X_LSB_ADDR
#define BNO055_GYR_DATA_X_LSB_ADDR   0x14
#define BNO055_GYR_DATA_X_MSB_ADDR   0x15
#define BNO055_GYR_DATA_Y_LSB_ADDR   0x16
#define BNO055_GYR_DATA_Y_MSB_ADDR   0x17
#define BNO055_GYR_DATA_Z_LSB_ADDR   0x18
#define BNO055_GYR_DATA_Z_MSB_ADDR   0x19
#define BNO055_GYR_DATA_ADDR         BNO055_GYR_DATA_X_LSB_ADDR
#define BNO055_EUL_HEADING_LSB_ADDR  0x1A
#define BNO055_EUL_HEADING_MSB_ADDR  0x1B
#define BNO055_EUL_ROLL_LSB_ADDR     0x1C
#define BNO055_EUL_ROLL_MSB_ADDR     0x1D
#define BNO055_EUL_PITCH_LSB_ADDR    0x1E
#define BNO055_EUL_PITCH_MSB_ADDR    0x1F
#define BNO055_QUA_DATA_W_LSB_ADDR   0x20
#define BNO055_QUA_DATA_W_MSB_ADDR   0x21
#define BNO055_QUA_DATA_X_LSB_ADDR   0x22
#define BNO055_QUA_DATA_X_MSB_ADDR   0x23
#define BNO055_QUA_DATA_Y_LSB_ADDR   0x24
#define BNO055_QUA_DATA_Y_MSB_ADDR   0x25
#define BNO055_QUA_DATA_Z_LSB_ADDR   0x26
#define BNO055_QUA_DATA_Z_MSB_ADDR   0x27
#define BNO055_LIA_DATA_X_LSB_ADDR   0x28
#define BNO055_LIA_DATA_X_MSB_ADDR   0x29
#define BNO055_LIA_DATA_Y_LSB_ADDR   0x2A
#define BNO055_LIA_DATA_Y_MSB_ADDR   0x2B
#define BNO055_LIA_DATA_Z_LSB_ADDR   0x2C
#define BNO055_LIA_DATA_Z_MSB_ADDR   0x2D
#define BNO055_GRV_DATA_X_LSB_ADDR   0x2E
#define BNO055_GRV_DATA_X_MSB_ADDR   0x2F
#define BNO055_GRV_DATA_Y_LSB_ADDR   0x30
#define BNO055_GRV_DATA_Y_MSB_ADDR   0x31
#define BNO055_GRV_DATA_Z_LSB_ADDR   0x32
#define BNO055_GRV_DATA_Z_MSB_ADDR   0x33
#define BNO055_TEMP_ADDR             0x34
#define BNO055_CALIB_STAT_ADDR       0x35
#define BNO055_ST_RESULT_ADDR        0x36
#define BNO055_INT_STA_ADDR          0x37
#define BNO055_SYS_CLK_STAT_ADDR     0x38
#define BNO055_SYS_STAT_ADDR         0x39
#define BNO055_SYS_ERR_ADDR          0x3A
#define BNO055_UNIT_SEL_ADDR         0x3B
#define BNO055_OPR_MODE_ADDR         0x3D
#define BNO055_PWR_MODE_ADDR         0x3E
#define BNO055_SYS_TRIGGER_ADDR      0x3F
#define BNO055_TEMP_SOURCE_ADDR      0x40
#define BNO055_AXIS_MAP_CONFIG_ADDR  0x41
#define BNO055_AXIS_MAP_SIGN_ADDR    0x42
#define BNO055_ACC_OFFSET_X_LSB_ADDR 0x55
#define BNO055_ACC_OFFSET_X_MSB_ADDR 0x56
#define BNO055_ACC_OFFSET_Y_LSB_ADDR 0x57
#define BNO055_ACC_OFFSET_Y_MSB_ADDR 0x58
#define BNO055_ACC_OFFSET_Z_LSB_ADDR 0x59
#define BNO055_ACC_OFFSET_Z_MSB_ADDR 0x5A
#define BNO055_MAG_OFFSET_X_LSB_ADDR 0x5B
#define BNO055_MAG_OFFSET_X_MSB_ADDR 0x5C
#define BNO055_MAG_OFFSET_Y_LSB_ADDR 0x5D
#define BNO055_MAG_OFFSET_Y_MSB_ADDR 0x5E
#define BNO055_MAG_OFFSET_Z_LSB_ADDR 0x5F
#define BNO055_MAG_OFFSET_Z_MSB_ADDR 0x60
#define BNO055_GYR_OFFSET_X_LSB_ADDR 0x61
#define BNO055_GYR_OFFSET_X_MSB_ADDR 0x62
#define BNO055_GYR_OFFSET_Y_LSB_ADDR 0x63
#define BNO055_GYR_OFFSET_Y_MSB_ADDR 0x64
#define BNO055_GYR_OFFSET_Z_LSB_ADDR 0x65
#define BNO055_GYR_OFFSET_Z_MSB_ADDR 0x66
#define BNO055_ACC_RADIUS_LSB_ADDR   0x67
#define BNO055_ACC_RADIUS_MSB_ADDR   0x68
#define BNO055_MAG_RADIUS_LSB_ADDR   0x69
#define BNO055_MAG_RADIUS_MSB_ADDR   0x6A
/* PAGE 1 */
#define BNO055_ACC_CONFIG_ADDR       0x08
#define BNO055_MAG_CONFIG_ADDR       0x09
#define BNO055_GYR_CONFIG_0_ADDR     0x0A
#define BNO055_GYR_CONFIG_1_ADDR     0x0B
#define BNO055_ACC_SLEEP_CONFIG_ADDR 0x0C
#define BNO055_GYR_SLEEP_CONFIG_ADDR 0x0D
#define BNO055_INT_MSK_ADDR          0x0F
#define BNO055_INT_EN_ADDR           0x10
#define BNO055_ACC_AM_THRES_ADDR     0x11
#define BNO055_ACC_INT_SETTINGS_ADDR 0x12
#define BNO055_ACC_HG_DURATION_ADDR  0x13
#define BNO055_ACC_HG_THRES_ADDR     0x14
#define BNO055_ACC_NM_THRES_ADDR     0x15
#define BNO055_ACC_NM_SET_ADDR       0x16
#define BNO055_GYR_INT_SETTINGS_ADDR 0x17
#define BNO055_GYR_HR_X_SET_ADDR     0x18
#define BNO055_GYR_DUR_X_ADDR        0x19
#define BNO055_GYR_HR_Y_SET_ADDR     0x1A
#define BNO055_GYR_DUR_Y_ADDR        0x1B
#define BNO055_GYR_HR_Z_SET_ADDR     0x1C
#define BNO055_GYR_DUR_Z_ADDR        0x1D
#define BNO055_GYR_AM_THRESH_ADDR    0x1E
#define BNO055_GYR_AM_SET_ADDR       0x1F

typedef struct {
	uint16_t ax;
	uint16_t ay;
	uint16_t az;

	uint16_t gx;
	uint16_t gy;
	uint16_t gz;

	uint16_t mx;
	uint16_t my;
	uint16_t mz;
} bno055_data;

typedef struct {
    I2C_HandleTypeDef* i2c_handle;
    uint8_t address;
    bno055_data* data;
} bno055_handle;

typedef enum {
    BNO055_PWR_Normal  = 0b00000000,
    BNO055_PWR_LowPwr  = 0b00000001,
    BNO055_PWR_Suspend = 0b00000010
} bno055_pwr_mode;

typedef enum {
    BNO055_OPR_Config       = 0b00000000,
    BNO055_OPR_ACCOnly      = 0b00000001,
    BNO055_OPR_MAGOnly      = 0b00000010,
    BNO055_OPR_GYROnly      = 0b00000011,
    BNO055_OPR_ACCMAG       = 0b00000100,
    BNO055_OPR_ACCGYR       = 0b00000101,
    BNO055_OPR_MAGGYR       = 0b00000110,
    BNO055_OPR_AMG          = 0b00000111,
    BNO055_OPR_IMU          = 0b00001000,
    BNO055_OPR_COMPASS      = 0b00001001,
    BNO055_OPR_M4G          = 0b00001010,
    BNO055_OPR_NDOF_FMC_OFF = 0b00001011,
    BNO055_OPR_NDOF         = 0b00001100
} bno055_opr_mode;

typedef enum {
    BNO055_UNITS_ACC_MS2 = 0b00000000,
    BNO055_UNITS_ACC_MG  = 0b00000001,
    BNO055_UNITS_GYR_DPS = 0b00000000,
    BNO055_UNITS_GYR_RPS = 0b00000010,
    BNO055_UNITS_EUL_DEG = 0b00000000,
    BNO055_UNITS_EUL_RAD = 0b00000100,
    BNO055_UNITS_TEMP_C  = 0b00000000,
    BNO055_UNITS_TEMP_F  = 0b00010000
} bno055_units;

typedef enum {
    ACC_G_RANGE_2G  = 0b00000000,
    ACC_G_RANGE_4G  = 0b00000001,
    ACC_G_RANGE_8G  = 0b00000010,
    ACC_G_RANGE_16G = 0b00000011
} acc_g_range;

typedef enum {
    ACC_BANDWIDTH_7_81_Hz  = 0b00000000,
    ACC_BANDWIDTH_15_63_Hz = 0b00000100,
    ACC_BANDWIDTH_31_25_Hz = 0b00001000,
    ACC_BANDWIDTH_62_5_Hz  = 0b00001100,
    ACC_BANDWIDTH_125_Hz   = 0b00010000,
    ACC_BANDWIDTH_250_Hz   = 0b00010100,
    ACC_BANDWIDTH_500_Hz   = 0b00011000,
    ACC_BANDWIDTH_1000_Hz  = 0b00011100
} acc_bandwidth;

typedef enum {
    ACC_OPR_Normal      = 0b00000000,
    ACC_OPR_Suspend     = 0b00100000,
    ACC_OPR_LowPwr1     = 0b01000000,
    ACC_OPR_Standby     = 0b01100000,
    ACC_OPR_LowPwr2     = 0b10000000,
    ACC_OPR_DeepSuspend = 0b10100000
} acc_opr_mode;

typedef enum {
    GYR_RANGE_2000DPS = 0b00000000,
    GYR_RANGE_1000DPS = 0b00000001,
    GYR_RANGE_500DPS  = 0b00000010,
    GYR_RANGE_250DPS  = 0b00000011,
    GYR_RANGE_125DPS  = 0b00000100
} gyr_range;

typedef enum {
    GYR_BANDWIDTH_523_Hz = 0b00000000,
    GYR_BANDWIDTH_230_Hz = 0b00001000,
    GYR_BANDWIDTH_116_Hz = 0b00010000,
    GYR_BANDWIDTH_47_Hz  = 0b00011000,
    GYR_BANDWIDTH_23_Hz  = 0b00100000,
    GYR_BANDWIDTH_12_Hz  = 0b00101000,
    GYR_BANDWIDTH_64_Hz  = 0b00110000,
    GYR_BANDWIDTH_32_Hz  = 0b00111000
} gyr_bandwidth;

typedef enum {
    GYR_OPR_Normal      = 0b00000000,
    GYR_OPR_FastPwrUp   = 0b00000001,
    GYR_OPR_DeepSuspend = 0b00000010,
    GYR_OPR_Suspend     = 0b00000011,
    GYR_OPR_AdvPwrSave  = 0b00000100
} gyr_opr_mode;

typedef enum {
    MAG_DATA_RATE_2_Hz  = 0b00000000,
    MAG_DATA_RATE_6_Hz  = 0b00000001,
    MAG_DATA_RATE_8_Hz  = 0b00000010,
    MAG_DATA_RATE_10_Hz = 0b00000011,
    MAG_DATA_RATE_15_Hz = 0b00000100,
    MAG_DATA_RATE_20_Hz = 0b00000101,
    MAG_DATA_RATE_25_Hz = 0b00000110,
    MAG_DATA_RATE_30_Hz = 0b00000111
} mag_data_rate;

typedef enum {
    MAG_OPR_LowPwr    = 0b00000000,
    MAG_OPR_Regular   = 0b00001000,
    MAG_OPR_EnRegular = 0b00010000,
    MAG_OPR_HighAcc   = 0b00011000
} mag_opr_mode;

typedef enum {
    MAG_PWR_Normal  = 0b00000000,
    MAG_PWR_Sleep   = 0b00100000,
    MAG_PWR_Suspend = 0b01000000,
    MAG_PWR_Force   = 0b01100000
} mag_pwr_mode;

typedef struct {
    bno055_pwr_mode pwr_mode;
    bno055_opr_mode opr_mode;

    struct {
        bno055_units acc;
        bno055_units gyr;
        bno055_units eul;
        bno055_units temp;
    } units;

    struct {
        acc_g_range g_range;
        acc_bandwidth bandwidth;
        acc_opr_mode opr_mode;
    } acc_config;

    struct {
        gyr_range range;
        gyr_bandwidth bandwidth;
        gyr_opr_mode opr_mode;
    } gyr_config;

    struct {
        mag_data_rate data_rate;
        mag_opr_mode opr_mode;
        mag_pwr_mode pwr_mode;
    } mag_config;
} bno055_config;

HAL_StatusTypeDef BNO055_WriteRegister(bno055_handle *bno055, uint8_t reg, uint8_t value);
HAL_StatusTypeDef BNO055_ReadRegister(bno055_handle *bno055, uint8_t reg, uint8_t* data, uint8_t size);

uint8_t BNO055_ReadChipID(bno055_handle *bno055);
uint8_t BNO055_ReadAccID(bno055_handle *bno055);
uint8_t BNO055_ReadGyrID(bno055_handle *bno055);
uint8_t BNO055_ReadMagID(bno055_handle *bno055);

void BNO055_SetPage(bno055_handle *bno055, uint8_t page);
void BNO055_SetPwrMode(bno055_handle *bno055, bno055_pwr_mode pwr_mode);
void BNO055_SetOprMode(bno055_handle *bno055, bno055_opr_mode opr_mode);
void BNO055_SetUnits(bno055_handle *bno055, bno055_units acc, bno055_units gyr, bno055_units eul, bno055_units temp);

void BNO055_ConfigureAcc(bno055_handle *bno055, acc_g_range g_range, acc_bandwidth bandwidth, acc_opr_mode opr_mode);
void BNO055_ConfigureGyr(bno055_handle *bno055, gyr_range range, gyr_bandwidth bandwidth, gyr_opr_mode opr_mode);
void BNO055_ConfigureMag(bno055_handle *bno055, mag_data_rate data_rate, mag_opr_mode opr_mode, mag_pwr_mode pwr_mode);

uint8_t BNO055_Init(bno055_handle *bno055, bno055_config* config);

void BNO055_ReadAcc(bno055_handle *bno055);
void BNO055_ReadGyr(bno055_handle *bno055);
void BNO055_ReadMag(bno055_handle *bno055);

#endif /* INC_DRIVERS_BNO055_H_ */
