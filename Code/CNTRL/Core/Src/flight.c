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
extern float vbat;

uint16_t calibration_samples = 0;
uint32_t last_sample_time = 0;

float Psum = 0.0f;
float quat_sum[4] = {0};

uint8_t calibrated = 0;
uint8_t calibration_set = 0;

uint32_t last_update_time = 0;

Kalman_2D state_kalman = {0};
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

void flight_calibrate() {
	if (!calibrated) {
		if (calibration_samples > NUM_CALIBRATION_SAMPLES) {
			calibrated = 1;
		} else {
			uint32_t elapsed = HAL_GetTick() - last_sample_time;
			if (elapsed >= CALIBRATION_SAMPLE_INTERVAL) {
				Psum += data.pressure;
				quat_sum[0] += orientation_quat[0];
				quat_sum[1] += orientation_quat[1];
				quat_sum[2] += orientation_quat[2];
				quat_sum[3] += orientation_quat[3];
				last_sample_time = HAL_GetTick();
				calibration_samples++;
			}
		}
	}
}

void flight_set_calibrations(flight_FSM* f) {
	if (!calibration_set && calibrated) {
		f->vars.P0 = Psum / NUM_CALIBRATION_SAMPLES;
		f->vars.quat0[0] = quat_sum[0] / NUM_CALIBRATION_SAMPLES;
		f->vars.quat0[1] = quat_sum[1] / NUM_CALIBRATION_SAMPLES;
		f->vars.quat0[2] = quat_sum[2] / NUM_CALIBRATION_SAMPLES;
		f->vars.quat0[3] = quat_sum[3] / NUM_CALIBRATION_SAMPLES;
		calibration_set = 1;
		f->calib_ready = 1;
		f->vars.time_DO_NOT_USE = HAL_GetTick();
	}
}

void flight_update_vars(flight_FSM* f) {
	if (!calibration_set) return;

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
	Kalman_2D_altitude_predict(&state_kalman, accel_z_filtered, PROCESS_ALPHA, PROCESS_MIN_VAR);

	// kalman update
	float baro_alt_raw = relative_altitude(data.pressure, f->vars.P0, data.temperature);
	baro_alt_filtered = ALTITUDE_ALPHA * baro_alt_raw + (1.0f - ALTITUDE_ALPHA) * baro_alt_filtered;
	Kalman_2D_altitude_update(&state_kalman, baro_alt_filtered, MEASUREMENT_ALPHA, MEASUREMENT_MIN_VAR);

	// update altitude, velocity, acceleration
	f->vars.alt = state_kalman.x[0];
	f->vars.v_vel = state_kalman.x[1];
	f->vars.v_accel = accel_z_filtered;

	// update max altitude
	if (f->vars.alt > f->vars.max_alt) f->vars.max_alt = f->vars.alt;

	// update times
	f->vars.time_DO_NOT_USE = timems;
	last_update_time = timems;
}

void flight_change_state(flight_FSM* f, flight_state state) {
	f->state = state;
	f->vars.state_entry_ms = HAL_GetTick();
}

void flight_initialize(flight_FSM* f) {
	flight_change_state(f, FLIGHT_STATE_COUNTDOWN);

	float Q[2][2] = {
		{0.01f, 0.0f},
		{0.0f, 0.2f}
	};

	float R = 0.5f;

	Kalman_2D_altitude_initialize(&state_kalman, Q, R);
}

void flight_update_state(flight_FSM* f) {
	uint32_t dt = HAL_GetTick() - f->vars.state_entry_ms;

	switch (f->state) {
	case FLIGHT_STATE_PAD:
		if (f->inputs.arm_request) {
			flight_change_state(f, FLIGHT_STATE_ARMED);
		}
		break;
	case FLIGHT_STATE_ARMED:
		if (f->inputs.start_countdown) {
			flight_change_state(f, FLIGHT_STATE_COUNTDOWN);
		}
		break;
	case FLIGHT_STATE_COUNTDOWN:
	    if (dt >= COUNTDOWN_TIME) {
	        f->signals.ignite_motor = 1;
	        flight_change_state(f, FLIGHT_STATE_BOOST);
	    }
	    break;
	case FLIGHT_STATE_BOOST:
	    if (f->vars.v_accel < BURNOUT_ACCEL_THRESH) {
	        f->flags.burnout = 1;

	        if (!f->inputs.motor_cont) {
	            flight_change_state(f, FLIGHT_STATE_MOTOR_FAIL);
	        } else {
	            flight_change_state(f, FLIGHT_STATE_BURNOUT);
	        }
	    }
	    break;
	case FLIGHT_STATE_MOTOR_FAIL:
	    if (dt < MOTOR_RETRY_TIME) {
	        f->signals.ignite_motor = 1;
	    } else {
	    	// TODO: abort flight or just continue with burnout???
	        flight_change_state(f, FLIGHT_STATE_BURNOUT);
	    }
	    break;
	case FLIGHT_STATE_BURNOUT:
	    if (fabsf(f->vars.v_vel) < APOGEE_VEL_THRESH) {
	        f->flags.apogee = 1;
	        f->signals.deploy_chute = 1;

	        if (!f->inputs.chute_cont) {
	            flight_change_state(f, FLIGHT_STATE_CHUTE_FAIL);
	        } else {
	            flight_change_state(f, FLIGHT_STATE_DESCENT);
	        }
	    }
	    break;
	case FLIGHT_STATE_CHUTE_FAIL: {
	    if (dt < CHUTE_RETRY_TIME) {
	        f->signals.deploy_chute = 1;
	    } else {
	        flight_change_state(f, FLIGHT_STATE_DESCENT);
	    }
	} break;
	case FLIGHT_STATE_DESCENT:
		if (fabsf(f->vars.v_vel) < TOUCHDOWN_VEL_THRESH) {
			f->flags.touchdown = 1;
			flight_change_state(f, FLIGHT_STATE_TOUCHDOWN);
		}
		break;
	case FLIGHT_STATE_TOUCHDOWN:
		if (dt > TOUCHDOWN_WAIT_TIME) {
			f->signals.log_data = 1;
			flight_change_state(f, FLIGHT_STATE_LOGGING);
		}
		break;
	case FLIGHT_STATE_LOGGING:
		if (f->inputs.logging_done) {
			flight_change_state(f, FLIGHT_STATE_READY);
		}
		break;
	case FLIGHT_STATE_READY:
		f->signals.signal_ready = 1;
		break;
	}
}
