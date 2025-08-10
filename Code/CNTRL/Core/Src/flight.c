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

float relative_altitude(float P, float P0, float T_C) {
	return (CONSTANT_R * (T_C + CONSTANT_C2K) / CONSTANT_g) * log(P0 / P);
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

	// update times
	uint32_t timems = HAL_GetTick();

	if (!f->flags.motor_ignited) {
		f->vars.launch_time = timems;
	} else {
		f->vars.flight_time = ((float)(timems - f->vars.launch_time)) / 1000.0f;
	}

	// update altitudes
	float alt = relative_altitude(data.pressure, f->vars.P0, data.temperature);
	if (f->vars.alt > f->vars.max_alt) f->vars.max_alt = f->vars.alt;

	// update velocities
	float dalt = alt - f->vars.alt;
	float dt = ((float)(timems - f->vars.time_DO_NOT_USE)) / 1000.0f;
	float v_vel = dalt / dt;

	// update acceleration
	float dv = v_vel - f->vars.v_vel;
	float baro_v_accel = dv / dt;

	f->vars.alt = alt;
	f->vars.v_vel = v_vel;
	f->vars.v_accel = baro_v_accel;
	f->vars.time_DO_NOT_USE = timems;

	// update voltage
	f->vars.vbat = vbat;

	last_update_time = HAL_GetTick();
}

void flight_initialize(flight_FSM* f) {
	f->state = FLIGHT_STATE_PAD;
	f->vars.state_entry_ms = HAL_GetTick();

//	flight_update_vars(f);
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
