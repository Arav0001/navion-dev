/*
 * util.c
 *
 *  Created on: May 29, 2025
 *      Author: aravs
 */

#include "util.h"

void raw_sensor_data_to_packet(raw_sensor_data* data, uint8_t* packet) {
	memset(packet, 0x00, sizeof(raw_sensor_data));
	memcpy(packet, data, sizeof(raw_sensor_data));
}
