/*
 * uart_dma.c
 *
 *  Created on: Jun 26, 2025
 *      Author: aravs
 */

#include "uart_dma.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* PACKET */
sensor_packet packet_A = {0};
sensor_packet packet_B = {0};

sensor_packet* volatile write_packet = &packet_A;
sensor_packet* volatile read_packet  = &packet_B;

volatile uint8_t data_ready = 0;

/* UART DMA */
uint8_t rx_data_buffer[PACKET_SIZE] = {0};
uint8_t rx_dma_buffer[RX_BUFFER_SIZE] = {0};

uint16_t rx_tail = 0;
uint16_t rx_head = 0;
uint16_t rx_bytes = 0;
uint16_t wrap_size = 0;

uint32_t rx_callback_counter = 0;
uint32_t rx_wrap_counter = 0;

uint32_t valid_packets = 0;
uint32_t invalid_packets = 0;
uint32_t corrupted_packets = 0;
uint32_t missed_packets = 0;

void initialize_uart_dma() {
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_dma_buffer, RX_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void uart_dma_rx_event_callback(uint16_t Size) {
	rx_callback_counter++;

	rx_tail = rx_head;

	if (Size == RX_BUFFER_SIZE) {
		rx_head = 0;
	} else {
		rx_head = Size;
	}

	if (wrap_size != 0) {
		rx_bytes = Size + wrap_size;

		if (rx_bytes != PACKET_SIZE) {
			invalid_packets++;
			wrap_size = 0;
			HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, SET);
			return;
		}

		memcpy(rx_data_buffer, &rx_dma_buffer[RX_BUFFER_SIZE - wrap_size], wrap_size);
		memcpy(&rx_data_buffer[wrap_size], rx_dma_buffer, Size);

		wrap_size = 0;
	} else {
		if (rx_tail > rx_head) {
			if (rx_head == 0) {
				wrap_size = RX_BUFFER_SIZE - rx_tail;
				if (wrap_size == PACKET_SIZE) {
					memcpy(rx_data_buffer, &rx_dma_buffer[rx_tail], PACKET_SIZE);
					wrap_size = 0;
				} else {
					rx_wrap_counter++;
					return;
				}
			} else {
				invalid_packets++;
				wrap_size = 0;
				HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, SET);
				return;
			}
		} else {
			rx_bytes = rx_head - rx_tail;

			if (rx_bytes == PACKET_SIZE) {
				memcpy(rx_data_buffer, &rx_dma_buffer[rx_tail], PACKET_SIZE);
			} else if (rx_bytes == 0) {
				return;
			} else {
				invalid_packets++;
				wrap_size = 0;
				HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, SET);
				return;
			}
		}
	}

	bytes_to_packet(rx_data_buffer, write_packet);
	uint8_t is_valid = validate_packet(write_packet);

	if (is_valid) {
		// Swap read/write buffers
		sensor_packet* temp = write_packet;
		write_packet = read_packet;
		read_packet = temp;

		data_ready = 1;
		valid_packets++;
		HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, RESET);
	} else {
		corrupted_packets++;
		HAL_GPIO_WritePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin, SET);
	}

	missed_packets = rx_callback_counter - (rx_wrap_counter + valid_packets + invalid_packets + corrupted_packets);
}

void uart_dma_error_callback() {
	HAL_UART_Abort(&huart1);

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_dma_buffer, RX_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

uint8_t uart_dma_is_data_ready() {
	return data_ready;
}

void uart_dma_reset_data_ready() {
	data_ready = 0;
}

sensor_packet* uart_dma_get_latest_packet() {
	return read_packet;
}

uint32_t uart_dma_get_valid_packets() {
	return valid_packets;
}

uint32_t uart_dma_get_invalid_packets() {
	return invalid_packets;
}

uint32_t uart_dma_get_corrupted_packets() {
	return corrupted_packets;
}

uint32_t uart_dma_get_missed_packets() {
	return missed_packets;
}
