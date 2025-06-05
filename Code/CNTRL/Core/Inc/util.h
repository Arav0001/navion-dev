/*
 * util.h
 *
 *  Created on: May 23, 2025
 *      Author: aravs
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include <string.h>

#include "main.h"

typedef struct {
	uint16_t ax;
	uint16_t ay;
	uint16_t az;

	uint16_t gx;
	uint16_t gy;
	uint16_t gz;

	uint16_t mx;
	uint16_t my;
	uint16_t mz;
} __attribute__((packed)) bno055_data;

typedef struct {
    uint32_t pressure;
    uint32_t temperature;
} __attribute__((packed)) bmp390_data;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t z;
} __attribute__((packed)) neom9n_data;

typedef struct {
	uint64_t time;

	bno055_data bno055;
	bmp390_data bmp390;
	neom9n_data neom9n;
} __attribute__((packed)) raw_sensor_data;

typedef struct {
	uint64_t time;

	float ax;
	float ay;
	float az;

	float gx;
	float gy;
	float gz;

	float mx;
	float my;
	float mz;

	float pressure;
	float temperature;
} __attribute__((packed)) sensor_data;

void packet_to_sensor_data(uint8_t* packet, sensor_data* data);

#endif /* INC_UTIL_H_ */
