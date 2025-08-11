/*
 * flight.c
 *
 *  Created on: Aug 9, 2025
 *      Author: aravs
 */

#include "flight.h"

#include <math.h>

#include "util.h"
#include "Drivers/pyro.h"

#include "control.h"

extern sensor_data data;
extern float orientation_quat[4];
extern pyro motor;
extern pyro parachute;
extern float vbat;

uint16_t pressure_samples = 0;
uint32_t last_sample_time = 0;
float Psum = 0.0f;
uint8_t pressure_calibrated = 0;
uint8_t pressure_set = 0;

uint32_t last_update_time = 0;

Kalman_2D kalman = {0};
float accel_z_filtered = 0.0f;
float baro_alt_filtered = 0.0f;

float relative_altitude(float P, float P0, float T_C) {
	return (CONSTANT_R * (T_C + CONSTANT_C2K) / CONSTANT_g) * log(P0 / P);
}

float gravity_compensated_accel(float orientation[4], float ax, float ay, float az) {
	float w = orientation[0];
	float x = orientation[1];
	float y = orientation[2];
	float z = orientation[3];

	// v' = q * v * q^-1
	// q * v
	float qv_w = -x * ax - y * ay - z * az;
	float qv_x =  w * ax + y * az - z * ay;
	float qv_y =  w * ay + z * ax - x * az;
	float qv_z =  w * az + x * ay - y * ax;

	// qv * q^-1
	float inv_w =  w;
	float inv_x = -x;
	float inv_y = -y;
	float inv_z = -z;

	float res_x = qv_w * inv_x + qv_x * inv_w + qv_y * inv_z - qv_z * inv_y;
	float res_y = qv_w * inv_y - qv_x * inv_z + qv_y * inv_w + qv_z * inv_x;
	float res_z = qv_w * inv_z + qv_x * inv_y - qv_y * inv_x + qv_z * inv_w;

	UNUSED(res_x);
	UNUSED(res_y);

	return res_z - CONSTANT_g;
}

void flight_calibrate_initial_pressure() {
	if (!pressure_calibrated) {
		if (pressure_samples > NUM_PRESSURE_SAMPLES) {
			pressure_calibrated = 1;
		} else {
			uint32_t elapsed = HAL_GetTick() - last_sample_time;
			if (elapsed >= PRESSURE_SAMPLE_INTERVAL) {
				Psum += data.pressure;
				last_sample_time = HAL_GetTick();
				pressure_samples++;
			}
		}
	}
}

void flight_set_calibrated_pressure(flight_FSM* f) {
	if (!pressure_set && pressure_calibrated) {
		f->vars.P0 = Psum / NUM_PRESSURE_SAMPLES;
		pressure_set = 1;
		f->vars.time_DO_NOT_USE = HAL_GetTick();
	}
}

void flight_update_vars(flight_FSM* f) {
	if (!pressure_set) return;

	uint32_t elapsed = HAL_GetTick() - last_update_time;
	if (elapsed < FSM_UPDATE_INTERVAL) return;

	uint32_t timems = HAL_GetTick();

	if (!f->flags.motor_ignited) {
		f->vars.launch_time = timems;
	} else {
		f->vars.flight_time = ((float)(timems - f->vars.launch_time)) / 1000.0f;
	}

	// kalman predict
	float accel_z_raw = gravity_compensated_accel(orientation_quat, data.ax, data.ay, data.az);
	accel_z_filtered = V_ACCEL_ALPHA * accel_z_raw + (1.0f - V_ACCEL_ALPHA) * accel_z_filtered;
	Kalman_2D_altitude_predict(&kalman, accel_z_filtered);

	// kalman update
	float baro_alt_raw = relative_altitude(data.pressure, f->vars.P0, data.temperature);
	baro_alt_filtered = ALTITUDE_ALPHA * baro_alt_raw + (1.0f - ALTITUDE_ALPHA) * baro_alt_filtered;
	Kalman_2D_altitude_update(&kalman, baro_alt_filtered);

	// update altitude, velocity, acceleration
	f->vars.alt = kalman.x[0];
	f->vars.v_vel = kalman.x[1];
	f->vars.v_accel = accel_z_filtered;

	// update max altitude
	if (f->vars.alt > f->vars.max_alt) f->vars.max_alt = f->vars.alt;

	// update times
	f->vars.time_DO_NOT_USE = timems;
	last_update_time = timems;
}

void flight_initialize(flight_FSM* f) {
	f->state = FLIGHT_STATE_PAD;
	f->vars.state_entry_ms = HAL_GetTick();

	float Q[2][2] = {
		{0.01f, 0.0f},
		{0.0f, 0.2f}
	};

	float R = 5.0f;

	Kalman_2D_altitude_initialize(&kalman, Q, R);
}

void flight_update_state(flight_FSM* f) {
	switch (f->state) {
	case FLIGHT_STATE_PAD:
		break;
	case FLIGHT_STATE_ARMED:
		break;
	case FLIGHT_STATE_COUNTDOWN:
		break;
	case FLIGHT_STATE_PYRO_FAIL:
		break;
	case FLIGHT_STATE_LIFTOFF:
		break;
	case FLIGHT_STATE_BOOST:
		break;
	case FLIGHT_STATE_BURNOUT:
		break;
	case FLIGHT_STATE_APOGEE:
		break;
	case FLIGHT_STATE_CHUTES:
		break;
	case FLIGHT_STATE_NOCHUTES:
		break;
	case FLIGHT_STATE_CNTRL_DESCENT:
		break;
	case FLIGHT_STATE_NOCNTRL_DESCENT:
		break;
	case FLIGHT_STATE_TOUCHDOWN:
		break;
	case FLIGHT_STATE_LOGGING:
		break;
	case FLIGHT_STATE_READY:
		break;
	}
}
