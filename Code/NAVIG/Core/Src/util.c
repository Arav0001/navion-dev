/*
 * util.c
 *
 *  Created on: May 29, 2025
 *      Author: aravs
 */

#include "util.h"

extern CRC_HandleTypeDef hcrc;

uint32_t calculate_crc32(uint8_t *data, size_t len) {
    size_t padded_len = (len + 3) & ~0x03;
    size_t buffer_len = 2 * PACKET_SIZE;
    uint8_t buffer[2 * PACKET_SIZE];

    if (padded_len > buffer_len) return 0;

    memset(buffer, 0, buffer_len);
    memcpy(buffer, data, len);

    return HAL_CRC_Calculate(&hcrc, (uint32_t*)buffer, padded_len >> 2);
}

void build_packet(sensor_packet* packet, raw_sensor_data* data) {
	packet->header = PACKET_HEADER;
	packet->data = *data;
	packet->crc = calculate_crc32((uint8_t*)&packet->data, sizeof(raw_sensor_data));
}

uint8_t validate_packet(sensor_packet *packet) {
    if (packet->header != PACKET_HEADER) return 0;
    uint32_t expected = calculate_crc32((uint8_t*)&packet->data, sizeof(raw_sensor_data));
    return (packet->crc == expected);
}

void packet_to_bytes(sensor_packet* packet, uint8_t* bytes) {
	memset(bytes, 0x00, sizeof(sensor_packet));
	memcpy(bytes, packet, sizeof(sensor_packet));
}
