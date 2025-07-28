/*
 * servo.h
 *
 *  Created on: Jun 21, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_SERVO_H_
#define INC_DRIVERS_SERVO_H_

#include "Drivers/pwm.h"
#include "main.h"

#define SERVO_DEFAULT_MAX_ANGLE 		180.0f
#define SERVO_DEFAULT_INIT_ANGLE 	0.0f
#define SERVO_DEFAULT_MIN_PW 		1.0f
#define SERVO_DEFAULT_MAX_PW 		2.0f
#define SERVO_DEFAULT_PERIOD 		20.0f

#define SERVO_GET_PWM_RESOLUTION(CLOCK_FREQ, PERIOD) (uint32_t)(((float)CLOCK_FREQ / 1000.0f) * PERIOD)

typedef enum {
	SERVO_POSITIVE = 1,
	SERVO_NEGATIVE = -1
} servo_direction;

typedef struct {
	const float MAX_ANGLE;  	// degrees
	const float INIT_ANGLE; 	// degrees
	const float MIN_PW;		// milliseconds
	const float MAX_PW; 		// milliseconds
	const float PERIOD;		// milliseconds

	const uint8_t DIRECTION;
} servo_config;

typedef struct {
	float angle; // degrees

	servo_config config;
	pwm_channel pwm;
} servo;

void servo_start(servo* s);
void servo_stop(servo* s);
void servo_set_angle(servo* s, float angle);

#endif /* INC_DRIVERS_SERVO_H_ */
