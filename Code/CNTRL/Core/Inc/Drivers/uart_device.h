/*
 * uart_device.h
 *
 *  Created on: May 31, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_UART_DEVICE_H_
#define INC_DRIVERS_UART_DEVICE_H_

#include "main.h"
#include "util.h"

#define UART_PACKET_SIZE (sizeof(raw_sensor_data) * 4)

typedef struct {
    UART_HandleTypeDef* huart;

    uint8_t rx_buffer[UART_PACKET_SIZE];
	uint8_t tx_buffer[UART_PACKET_SIZE];

	uint8_t rx_complete;
	uint8_t tx_complete;

	uint16_t rx_tail;
	uint16_t rx_head;
} uart_device;

void UART_Device_DMA_EnableIdleInterrupt(uart_device* device);

void UART_Device_Init(uart_device* device, UART_HandleTypeDef* huart);
void UART_Device_Transmit_DMA(uart_device* device, uint8_t* data);
void UART_Device_Receive_DMA(uart_device* device);
void UART_RxCpltCallback(uart_device* device);
void UART_TxCpltCallback(uart_device* device);
void UART_ErrorCallback(uart_device* device);

#endif /* INC_DRIVERS_UART_DEVICE_H_ */
