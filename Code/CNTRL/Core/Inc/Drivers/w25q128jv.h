/*
 * w25q128jv.h
 *
 *  Created on: Mar 23, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_W25Q128JV_H_
#define INC_DRIVERS_W25Q128JV_H_

#include "main.h"

typedef struct {
    SPI_HandleTypeDef *spi_handle;
} w25q128jv_handle;

// Standard SPI Instructions
#define W25Q128JV_WRITE_ENABLE                 0x06
#define W25Q128JV_VOLATILE_SR_WRITE_ENABLE     0x50
#define W25Q128JV_WRITE_DISABLE                0x04
#define W25Q128JV_RELEASE_POWERDOWN_ID         0xAB
#define W25Q128JV_READ_MANUFACTURER_DEVICE_ID  0x90
#define W25Q128JV_READ_JEDEC_ID                0x9F
#define W25Q128JV_READ_UNIQUE_ID               0x4B
#define W25Q128JV_READ_DATA                    0x03
#define W25Q128JV_FAST_READ                    0x0B
#define W25Q128JV_PAGE_PROGRAM                 0x02
#define W25Q128JV_SECTOR_ERASE_4KB             0x20
#define W25Q128JV_BLOCK_ERASE_32KB             0x52
#define W25Q128JV_BLOCK_ERASE_64KB             0xD8
#define W25Q128JV_CHIP_ERASE                   0xC7 // Note: 0x60 is an alternative
#define W25Q128JV_READ_STATUS_REGISTER_1       0x05
#define W25Q128JV_WRITE_STATUS_REGISTER_1      0x01
#define W25Q128JV_READ_STATUS_REGISTER_2       0x35
#define W25Q128JV_WRITE_STATUS_REGISTER_2      0x31
#define W25Q128JV_READ_STATUS_REGISTER_3       0x15
#define W25Q128JV_WRITE_STATUS_REGISTER_3      0x11
#define W25Q128JV_READ_SFDP_REGISTER           0x5A
#define W25Q128JV_ERASE_SECURITY_REGISTER      0x44
#define W25Q128JV_PROGRAM_SECURITY_REGISTER    0x42
#define W25Q128JV_READ_SECURITY_REGISTER       0x48
#define W25Q128JV_GLOBAL_BLOCK_LOCK            0x7E
#define W25Q128JV_GLOBAL_BLOCK_UNLOCK          0x98
#define W25Q128JV_READ_BLOCK_LOCK              0x3D
#define W25Q128JV_INDIVIDUAL_BLOCK_LOCK        0x36
#define W25Q128JV_INDIVIDUAL_BLOCK_UNLOCK      0x39
#define W25Q128JV_ERASE_PROGRAM_SUSPEND        0x75
#define W25Q128JV_ERASE_PROGRAM_RESUME         0x7A
#define W25Q128JV_POWER_DOWN                   0xB9
#define W25Q128JV_ENABLE_RESET                 0x66
#define W25Q128JV_RESET_DEVICE                 0x99

void W25Q128JV_Reset(w25q128jv_handle *flash);
uint32_t W25Q128JV_Read_ID(w25q128jv_handle *flash);

void W25Q128JV_Read(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t* data);
void W25Q128JV_FastRead(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t* data);

void W25Q128JV_Erase_Sector(w25q128jv_handle *flash, uint16_t sector);
void W25Q128JV_Erase_Chip(w25q128jv_handle *flash);

void W25Q128JV_Write(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t* data);

#endif /* INC_DRIVERS_W25Q128JV_H_ */
