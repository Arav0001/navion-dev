/*
 * util.c
 *
 *  Created on: May 29, 2025
 *      Author: aravs
 */

#include "util.h"
#include "config.h"

#define ACC_SCALER 0.01f
#define GYR_SCALER 0.00111111111f
#define MAG_SCALER 0.0625f

#define PRESSURE_SCALER 0.01f
#define TEMPERATURE_SCALER 0.01f

const float ACC_BIAS[3] = {-0.076000005f, -0.0310000014f, 0.0420000032f};
const float ACC_SCALE_FACTOR[3][3] = {
	{0.994706213f, 0.0f, 0.0f},
	{0.0f, 0.996235013f, 0.0f},
	{0.0f, 0.0f, 0.981081963f}
};

const float GYR_BIAS[3] = {-0.107596666f, -0.0478107929f, -0.147472739f};

const float MAG_BIAS[3] = {-45.7000008f, -60.7999992f, 37.6000023f};
const float MAG_SCALE_FACTOR[3][3] = {
	{1.54200006f, 0.0960000008f, 0.00600000005f},
	{0.0960000008f, 1.27600002f, -0.128000006f},
	{0.00600000005f, -0.128000006f, 1.65100002f}
};

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

	data->pressure = PRESSURE_SCALER * raw_data->bmp390.pressure;
	data->temperature = TEMPERATURE_SCALER * raw_data->bmp390.temperature;

	// swap x-y
	data->ax = ACC_SCALER * (int16_t)raw_data->bno055.ay;
	data->gx = GYR_SCALER * (int16_t)raw_data->bno055.gy;
	data->mx = MAG_SCALER * (int16_t)raw_data->bno055.my;

	// swap x-y
	data->ay = ACC_SCALER * (int16_t)raw_data->bno055.ax;
	data->gy = GYR_SCALER * (int16_t)raw_data->bno055.gx;
	data->my = MAG_SCALER * (int16_t)raw_data->bno055.mx;

	// invert z
	data->az = -ACC_SCALER * (int16_t)raw_data->bno055.az;
	data->gz = -GYR_SCALER * (int16_t)raw_data->bno055.gz;
	data->mz = -MAG_SCALER * (int16_t)raw_data->bno055.mz;

	// remove biases
#ifndef CALIBRATE_ACC
	data->ax = (data->ax / CONSTANT_g - ACC_BIAS[0]) * ACC_SCALE_FACTOR[0][0] * CONSTANT_g;
	data->ay = (data->ay / CONSTANT_g - ACC_BIAS[1]) * ACC_SCALE_FACTOR[1][1] * CONSTANT_g;
	data->az = (data->az / CONSTANT_g - ACC_BIAS[2]) * ACC_SCALE_FACTOR[2][2] * CONSTANT_g;
#endif

#ifndef CALIBRATE_GYR
	data->gx = (data->gx - GYR_BIAS[0] / RAD_TO_DEG);
	data->gy = (data->gy - GYR_BIAS[1] / RAD_TO_DEG);
	data->gz = (data->gz - GYR_BIAS[2] / RAD_TO_DEG);
#endif

#ifndef CALIBRATE_MAG
	data->mx = (data->mx - MAG_BIAS[0]);
	data->my = (data->my - MAG_BIAS[1]);
	data->mz = (data->mz - MAG_BIAS[2]);

	float cal_mx = 	data->mx * MAG_SCALE_FACTOR[0][0] +
					data->my * MAG_SCALE_FACTOR[0][1] +
					data->mz * MAG_SCALE_FACTOR[0][2];

	float cal_my = 	data->mx * MAG_SCALE_FACTOR[1][0] +
					data->my * MAG_SCALE_FACTOR[1][1] +
					data->mz * MAG_SCALE_FACTOR[1][2];

	float cal_mz = 	data->mx * MAG_SCALE_FACTOR[2][0] +
					data->my * MAG_SCALE_FACTOR[2][1] +
					data->mz * MAG_SCALE_FACTOR[2][2];

	data->mx = cal_mx;
	data->my = cal_my;
	data->mz = cal_mz;
#endif
}

void bytes_to_packet(uint8_t* bytes, sensor_packet* packet) {
	memcpy(packet, bytes, sizeof(sensor_packet));
}

const char* pyro_state_to_str(uint8_t state) {
    static const char* names[] = {
        "STANDBY",
        "FIRING",
        "CONFIRMED",
        "TIMEOUT",
        "BROKEN"
    };

    if (state >= PYRO_STANDBY && state <= PYRO_BROKEN) {
        return names[state];
    }

    return "UNKNOWN";
}
