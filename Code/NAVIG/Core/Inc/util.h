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

typedef struct {
	uint64_t time;

	bno055_data bno055;
	bmp390_data bmp390;
	neom9n_data neom9n;
} __attribute__((packed)) sensor_data;

// currently 48 byte packet

void sensor_data_to_packet(sensor_data* data, uint8_t* packet) {
	memset(packet, 0x00, sizeof(sensor_data));
	memcpy(packet, data, sizeof(sensor_data));
}

void packet_to_sensor_data(uint8_t* packet, sensor_data* data) {
	memcpy(data, packet, sizeof(sensor_data));
}

#endif /* INC_UTIL_H_ */
