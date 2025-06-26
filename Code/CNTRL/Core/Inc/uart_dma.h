/*
 * uart_dma.h
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#ifndef INC_UART_DMA_H_
#define INC_UART_DMA_H_

#include "main.h"

#include <string.h>

#include "util.h"

#define RX_BUFFER_SIZE (256 * PACKET_SIZE)

void initialize_uart_dma();

void uart_dma_rx_event_callback(uint16_t Size);
void uart_dma_error_callback();

uint8_t uart_dma_is_data_ready();
void uart_dma_reset_data_ready();
sensor_packet* uart_dma_get_latest_packet();

uint32_t uart_dma_get_valid_packets();
uint32_t uart_dma_get_invalid_packets();
uint32_t uart_dma_get_corrupted_packets();
uint32_t uart_dma_get_missed_packets();

#endif /* INC_UART_DMA_H_ */
