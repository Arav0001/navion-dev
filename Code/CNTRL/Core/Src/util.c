/*
 * util.c
 *
 *  Created on: May 29, 2025
 *      Author: aravs
 */

#include "util.h"

#define ACC_SCALER 0.01f
#define GYR_SCALER 0.0625f
#define MAG_SCALER 0.0625f

#define PRESSURE_SCALER 0.01f
#define TEMPERATURE_SCALER 0.01f

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

void process_raw_sensor_data(raw_sensor_data* raw_data, sensor_data* data) {
	data->time = raw_data->time;

	data->ax = ACC_SCALER * (int16_t)raw_data->bno055.ax;
	data->ay = ACC_SCALER * (int16_t)raw_data->bno055.ay;
	data->az = ACC_SCALER * (int16_t)raw_data->bno055.az;

	data->gx = GYR_SCALER * (int16_t)raw_data->bno055.gx;
	data->gy = GYR_SCALER * (int16_t)raw_data->bno055.gy;
	data->gz = GYR_SCALER * (int16_t)raw_data->bno055.gz;

	data->mx = MAG_SCALER * (int16_t)raw_data->bno055.mx;
	data->my = MAG_SCALER * (int16_t)raw_data->bno055.my;
	data->mz = MAG_SCALER * (int16_t)raw_data->bno055.mz;

	data->pressure = PRESSURE_SCALER * raw_data->bmp390.pressure;
	data->temperature = TEMPERATURE_SCALER * raw_data->bmp390.temperature;
}

void bytes_to_packet(uint8_t* bytes, sensor_packet* packet) {
	memcpy(packet, bytes, sizeof(sensor_packet));
}
