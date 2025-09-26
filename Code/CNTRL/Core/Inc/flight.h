/*
 * flight.h
 *
 *  Created on: Aug 9, 2025
 *      Author: aravs
 */

#ifndef INC_FLIGHT_H_
#define INC_FLIGHT_H_

#include "main.h"

#define LIFTOFF_ACCEL_THRESH 	2.0f
#define BURNOUT_ACCEL_THRESH 	1.0f
#define APOGEE_VEL_THRESH 		0.5f
#define TOUCHDOWN_VEL_THRESH		0.25f

#define COUNTDOWN_TIME		10000
#define MOTOR_RETRY_TIME		2000
#define CHUTE_RETRY_TIME		3000
#define TOUCHDOWN_WAIT_TIME	500

#define NUM_CALIBRATION_SAMPLES 1000
#define CALIBRATION_SAMPLE_INTERVAL 10

#define FSM_UPDATE_INTERVAL 10

#define V_ACCEL_ALPHA 		0.1f
#define ALTITUDE_ALPHA 		0.1f

#define PROCESS_ALPHA			0.01f
#define PROCESS_MIN_VAR		1e-4f
#define MEASUREMENT_ALPHA	0.01f
#define MEASUREMENT_MIN_VAR	0.01f

typedef enum {
	FLIGHT_STATE_PAD,
	FLIGHT_STATE_ARMED,
	FLIGHT_STATE_COUNTDOWN,
	FLIGHT_STATE_MOTOR_FAIL,
	FLIGHT_STATE_BOOST,
	FLIGHT_STATE_BURNOUT,
	FLIGHT_STATE_CHUTE_FAIL,
	FLIGHT_STATE_DESCENT,
	FLIGHT_STATE_TOUCHDOWN,
	FLIGHT_STATE_LOGGING,
	FLIGHT_STATE_READY
} flight_state;

typedef struct {
	uint32_t launch_time;
	float flight_time;
	uint32_t time_DO_NOT_USE;

	float P0;
	float quat0[4];

	float alt;
	float max_alt;

	float v_vel;
	float v_accel;

	uint32_t state_entry_ms;
} flight_state_vars;

typedef struct {
	uint8_t armed				: 1;
	uint8_t countdown_started	: 1;
	uint8_t motor_ignited		: 1;
	uint8_t burnout				: 1;
	uint8_t apogee				: 1;
	uint8_t chutes				: 1;
	uint8_t chutes_conf			: 1;
	uint8_t touchdown			: 1;
} flight_event_flags;

typedef struct {
	uint8_t arm_request			: 1;
	uint8_t start_countdown		: 1;

	uint8_t motor_cont			: 1;
	uint8_t chute_cont			: 1;

	uint8_t logging_done		: 1;
} flight_inputs;

typedef struct {
    uint8_t ignite_motor      : 1;
    uint8_t deploy_chute      : 1;
    uint8_t log_data          : 1;
    uint8_t signal_ready      : 1;
} flight_signals;

typedef struct {
	flight_state state;
	flight_state_vars vars;
	flight_event_flags flags;
	flight_inputs inputs;
	flight_signals signals;
	uint8_t calib_ready;
} flight_FSM;

void flight_calibrate();
void flight_set_calibrations(flight_FSM* f);

void flight_update_vars(flight_FSM* f);
void flight_initialize(flight_FSM* f);

void flight_update_state(flight_FSM* f);

#endif /* INC_FLIGHT_H_ */
