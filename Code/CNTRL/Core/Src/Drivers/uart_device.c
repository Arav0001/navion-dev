/*
 * uart_device.c
 *
 *  Created on: May 31, 2025
 *      Author: aravs
 */

#include "Drivers/uart_device.h"

#include <string.h>

void UART_Device_DMA_EnableIdleInterrupt(uart_device* device) {
	SET_BIT(device->huart->Instance->CR1, USART_CR1_IDLEIE);
}

void UART_Device_Init(uart_device* device, UART_HandleTypeDef* huart) {
	device->huart = huart;
	device->rx_complete = 0;
	device->tx_complete = 1;
}

void UART_Device_Transmit_DMA(uart_device* device, uint8_t* data) {
	memcpy(device->tx_buffer, data, UART_PACKET_SIZE);

	device->tx_complete = 0;
	HAL_UART_Transmit_DMA(device->huart, device->tx_buffer, UART_PACKET_SIZE);
}

void UART_Device_Receive_DMA(uart_device* device) {
	device->rx_complete = 0;
	HAL_UART_Receive_DMA(device->huart, device->rx_buffer, UART_PACKET_SIZE);
}

void UART_RxCpltCallback(uart_device* device) {
	device->rx_complete = 1;
}

void UART_TxCpltCallback(uart_device* device) {
	device->tx_complete = 1;
}

void UART_ErrorCallback(uart_device* device) {
	device->rx_tail = 0;
	HAL_UART_Abort(device->huart);
	UART_Device_DMA_EnableIdleInterrupt(device);

	// Begin receive operations
	UART_Device_Receive_DMA(device);
	device->rx_head = 0;
}
