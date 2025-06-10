/*
 * util.h
 *
 *  Created on: May 23, 2025
 *      Author: aravs
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "Drivers/bno055.h"
#include "Drivers/bmp390.h"
#include "Drivers/neom9n.h"

#include <string.h>

#define PACKET_HEADER  0xABCD

typedef struct {
	uint64_t time;

	bno055_data bno055;
	bmp390_data bmp390;
	neom9n_data neom9n;
} __attribute__((packed)) raw_sensor_data;

typedef struct {
	uint16_t header;
	raw_sensor_data data;
	uint32_t crc;
} __attribute__((packed)) sensor_packet;

#define PACKET_SIZE (sizeof(sensor_packet))

uint32_t calculate_crc32(uint8_t *data, size_t len);

void build_packet(sensor_packet* packet, raw_sensor_data* data);
uint8_t validate_packet(sensor_packet *packet);

void packet_to_bytes(sensor_packet* packet, uint8_t* bytes);

#endif /* INC_UTIL_H_ */
