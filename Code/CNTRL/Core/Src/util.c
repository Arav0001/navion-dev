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

void packet_to_sensor_data(uint8_t* packet, sensor_data* data) {
	raw_sensor_data raw_data;

	memcpy(&raw_data, packet, sizeof(raw_sensor_data));

	data->time = raw_data.time;

	data->ax = ACC_SCALER * raw_data.bno055.ax;
	data->ay = ACC_SCALER * raw_data.bno055.ay;
	data->az = ACC_SCALER * raw_data.bno055.az;

	data->gx = GYR_SCALER * raw_data.bno055.gx;
	data->gy = GYR_SCALER * raw_data.bno055.gy;
	data->gz = GYR_SCALER * raw_data.bno055.gz;

	data->mx = MAG_SCALER * raw_data.bno055.mx;
	data->my = MAG_SCALER * raw_data.bno055.my;
	data->mz = MAG_SCALER * raw_data.bno055.mz;

	data->pressure = raw_data.bmp390.pressure;
	data->temperature = raw_data.bmp390.temperature;
}
