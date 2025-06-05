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
} __attribute__((packed)) raw_sensor_data;

void raw_sensor_data_to_packet(raw_sensor_data* data, uint8_t* packet);

#endif /* INC_UTIL_H_ */
