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
#define CHUTE_CNTRL_DESCENT		5.0f
#define CHUTE_NOCNTRL_DESCENT	10.0f
#define TOUCHDOWN_BEL_THRESH		0.5f;

#define NUM_PRESSURE_SAMPLES 500
#define PRESSURE_SAMPLE_INTERVAL 10

#define FSM_UPDATE_INTERVAL 10

typedef enum {
	FLIGHT_STATE_PAD,
	FLIGHT_STATE_ARMED,
	FLIGHT_STATE_COUNTDOWN,
	FLIGHT_STATE_PYRO_FAIL,
	FLIGHT_STATE_LIFTOFF,
	FLIGHT_STATE_BOOST,
	FLIGHT_STATE_BURNOUT,
	FLIGHT_STATE_APOGEE,
	FLIGHT_STATE_CHUTES,
	FLIGHT_STATE_NOCHUTES,
	FLIGHT_STATE_CNTRL_DESCENT,
	FLIGHT_STATE_NOCNTRL_DESCENT,
	FLIGHT_STATE_TOUCHDOWN,
	FLIGHT_STATE_LOGGING,
	FLIGHT_STATE_READY
} flight_state;

typedef struct {
	uint32_t launch_time;
	float flight_time;
	uint32_t time_DO_NOT_USE;

	float P0;

	float alt;
	float max_alt;

	float v_vel;
	float v_accel;

	float vbat;

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
	flight_state state;
	flight_state_vars vars;
	flight_event_flags flags;
} flight_FSM;

void flight_calibrate_initial_pressure();
void flight_set_calibrated_pressure(flight_FSM* f);

void flight_update_vars(flight_FSM* f);
void flight_initialize(flight_FSM* f);

void flight_update_state(flight_FSM* f);

#endif /* INC_FLIGHT_H_ */
