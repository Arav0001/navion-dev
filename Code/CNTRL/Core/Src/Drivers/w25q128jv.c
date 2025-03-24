/*
 * w25q128jv.c
 *
 *  Created on: Mar 23, 2025
 *      Author: aravs
 */

#include "Drivers/w25q128jv.h"

// TODO: fix these pin numbers
#define W25Q128JV_CSB_GPIO_PORT SPI2_CSB_GPIO_Port
#define W25Q128JV_CSB_PIN SPI2_CSB_Pin

void W25Q128JV_SPI_Write(w25q128jv_handle *flash, uint8_t* data, uint16_t size) {
    HAL_SPI_Transmit(flash->spi_handle, data, size, 1000);
}

void W25Q128JV_SPI_Read(w25q128jv_handle *flash, uint8_t* data, uint16_t size) {
    HAL_SPI_Receive(flash->spi_handle, data, size, 1000);
}

void W25Q128JV_Select(w25q128jv_handle *flash) {
    HAL_GPIO_WritePin(W25Q128JV_CSB_GPIO_PORT, W25Q128JV_CSB_PIN, 0);
}

void W25Q128JV_Deselect(w25q128jv_handle *flash) {
    HAL_GPIO_WritePin(W25Q128JV_CSB_GPIO_PORT, W25Q128JV_CSB_PIN, 1);
}

void W25Q128JV_Reset(w25q128jv_handle *flash) {
    uint8_t instructionBuf[2] = {
        W25Q128JV_ENABLE_RESET,
        W25Q128JV_RESET_DEVICE
    };

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 2);
    W25Q128JV_Deselect(flash);

    HAL_Delay(100);
}

uint32_t W25Q128JV_Read_ID(w25q128jv_handle *flash) {
    uint8_t instructionBuf[1] = {
        W25Q128JV_READ_JEDEC_ID
    };

    uint8_t readBuf[3];

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 1);
    W25Q128JV_SPI_Read(flash, readBuf, 3);
    W25Q128JV_Deselect(flash);

    return readBuf[0] << 16 | readBuf[1] << 8 | readBuf[2];
}

void W25Q128JV_Read(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t* data) {
    uint32_t address = page * 256 + offset;

    uint8_t instructionBuf[4] = {
        W25Q128JV_READ_DATA,
        (address >> 16) & 0xFF,
        (address >> 8) & 0xFF,
        (address) & 0xFF
    };

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 4);
    W25Q128JV_SPI_Read(flash, data, size);
    W25Q128JV_Deselect(flash);
}

void W25Q128JV_FastRead(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t* data) {
    uint32_t address = page * 256 + offset;

    uint8_t instructionBuf[5] = {
        W25Q128JV_READ_DATA,
        (address >> 16) & 0xFF,
        (address >> 8) & 0xFF,
        (address) & 0xFF,
        0xFF
    };

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 5);
    W25Q128JV_SPI_Read(flash, data, size);
    W25Q128JV_Deselect(flash);
}

void W25Q128JV_Write_Enable(w25q128jv_handle *flash) {
    uint8_t instructionBuf[1] = {
        W25Q128JV_WRITE_ENABLE
    };

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 1);
    W25Q128JV_Deselect(flash);

    HAL_Delay(5);
}

void W25Q128JV_Write_Disable(w25q128jv_handle *flash) {
    uint8_t instructionBuf[1] = {
        W25Q128JV_WRITE_DISABLE
    };

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 1);
    W25Q128JV_Deselect(flash);

    HAL_Delay(5);
}

void W25Q128JV_Erase_Sector(w25q128jv_handle *flash, uint16_t sector) {
    uint32_t address = sector * 16 * 256;

    uint8_t instructionBuf[4] = {
        W25Q128JV_SECTOR_ERASE_4KB,
        (address >> 16) & 0xFF,
        (address >> 8) & 0xFF,
        (address) & 0xFF
    };

    W25Q128JV_Write_Enable(flash);

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 4);
    W25Q128JV_Deselect(flash);

    HAL_Delay(450);

    W25Q128JV_Write_Disable(flash);
}

void W25Q128JV_Erase_Chip(w25q128jv_handle *flash) {
    uint8_t instructionBuf[1] = {
        W25Q128JV_CHIP_ERASE
    };

    W25Q128JV_Write_Enable(flash);

    W25Q128JV_Select(flash);
    W25Q128JV_SPI_Write(flash, instructionBuf, 1);
    W25Q128JV_Deselect(flash);

    HAL_Delay(200000);

    W25Q128JV_Write_Disable(flash);
}

void W25Q128JV_Write(w25q128jv_handle *flash, uint32_t page, uint32_t offset, uint32_t size, uint8_t *data) {
	uint8_t instructionBuf[260];

	uint32_t startPage = page;
	uint32_t endPage  = startPage + (size + offset - 1) / 256;

	uint16_t startSector  = startPage / 16;
	uint16_t endSector  = endPage / 16;

	for (uint16_t i = 0; i < endSector - startSector + 1; i++) W25Q128JV_Erase_Sector(flash, startSector++);

	uint32_t dataPosition = 0;

	for (uint32_t i = 0; i < endPage - startPage + 1; i++) {
		uint32_t writeAddress = (startPage * 256) + offset;

		uint16_t bytesRemaining = 256 - offset;
        if (size + offset < 256) bytesRemaining = size;

		uint32_t index = 4;

		W25Q128JV_Write_Enable(flash);

        instructionBuf[0] = W25Q128JV_PAGE_PROGRAM;
        instructionBuf[1] = (writeAddress >> 16) & 0xFF;
        instructionBuf[2] = (writeAddress >> 8) & 0xFF;
        instructionBuf[3] = (writeAddress) & 0xFF;

		for (uint16_t j = 0; j < bytesRemaining; j++) instructionBuf[index++] = data[j + dataPosition];

		W25Q128JV_Select(flash);
        W25Q128JV_SPI_Write(flash, instructionBuf, bytesRemaining + index);
        W25Q128JV_Deselect(flash);

		startPage++;
		offset = 0;
		size -= bytesRemaining;
		dataPosition += bytesRemaining;

		HAL_Delay(5);

		W25Q128JV_Write_Disable(flash);
	}
}

// void W25Q128JV_Write(uint32_t address, uint32_t size, uint8_t* data) {
//     uint8_t instructionBuf[260];

//     const uint32_t startPage = address / 256;
//     const uint32_t endPage = (address + size) / 256;

//     const uint32_t startSector = startPage / 16;
//     const uint32_t endSector = endPage / 16;

//     // erase all sectors which will be written to
//     for (uint32_t sector = startSector; sector <= endSector; sector++) {
//         W25Q128JV_Erase_Sector(sector);
//     }

//     uint32_t writeAddress = address;
//     uint32_t bytesWritten = 0;

//     for (uint32_t page = 0; page < endPage - startPage + 1; page++) {
//         instructionBuf[0] = W25Q128JV_PAGE_PROGRAM;
//         instructionBuf[1] = (writeAddress >> 16) & 0xFF;
//         instructionBuf[2] = (writeAddress >> 8) & 0xFF;
//         instructionBuf[3] = (writeAddress) & 0xFF;

//         uint8_t bytesToWrite = 256 - writeAddress % 256;
//         if (size - bytesWritten < bytesToWrite) bytesToWrite = size - bytesWritten;

//         for (uint8_t byte = 0; byte < bytesToWrite; byte++) {
//             instructionBuf[4 + byte] = data[bytesWritten + byte];
//         }

//         W25Q128JV_Write_Enable();

//         W25Q128JV_Select();
//         W25Q128JV_SPI_Write(instructionBuf, 4 + bytesToWrite);
//         W25Q128JV_Deselect();

//         HAL_Delay(5);

//         W25Q128JV_Write_Disable();

//         writeAddress += bytesToWrite;
//         bytesWritten += bytesToWrite;
//     }
// }
