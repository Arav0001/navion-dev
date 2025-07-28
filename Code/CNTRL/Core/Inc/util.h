/*
 * util.h
 *
 *  Created on: May 23, 2025
 *      Author: aravs
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include <string.h>
#include <math.h>

#include "main.h"

#include "Drivers/servo.h"
#include "Drivers/pyro.h"

#define RAD_TO_DEG 180.0f / M_PI
#define CONSTANT_g 9.8067f

#define PACKET_HEADER  0xABCD

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
    uint64_t pressure;
    int64_t temperature;
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
	uint16_t header;
	raw_sensor_data data;
	uint32_t crc;
} __attribute__((packed)) sensor_packet;

#define PACKET_SIZE (sizeof(sensor_packet))

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

typedef struct {
	float T_plus;

	float vbat;

	struct {
		float x;
		float y;
		float z;
	} acc;

	struct {
		float x;
		float y;
		float z;
	} gyr;

	struct {
		float x;
		float y;
		float z;
	} mag;

	struct {
		float w;
		float x;
		float y;
		float z;
	} quat;

	struct {
		float x;
		float y;
	} tvc;

	struct {
		uint8_t motor;
		uint8_t parachute;
	} pyro;
} __attribute__((packed)) rocket_data;

uint32_t calculate_crc32(uint8_t *data, size_t len);

void build_packet(sensor_packet* packet, raw_sensor_data* data);
uint8_t validate_packet(sensor_packet *packet);

void process_raw_sensor_data(raw_sensor_data* raw_data, sensor_data* data);
void bytes_to_packet(uint8_t* bytes, sensor_packet* packet);

const char* pyro_state_to_str(uint8_t state);

#endif /* INC_UTIL_H_ */
