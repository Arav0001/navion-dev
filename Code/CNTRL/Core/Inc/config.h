/*
 * config.h
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <stdint.h>
#include <string.h>

//#define CALIBRATE_ACC
//#define CALIBRATE_GYR
//#define CALIBRATE_MAG

#if defined(CALIBRATE_ACC) || defined(CALIBRATE_GYR) || defined(CALIBRATE_MAG)
    #define CALIBRATE
#endif

#define TOKEN_STRING_SIZE 64
#define MAX_LINE_LENGTH 128

#define SETTINGS_FILE "navion.settings"

typedef int32_t	INT_t;
typedef float		FLT_t;
typedef char		STR_t[TOKEN_STRING_SIZE];

extern uint8_t CONFIG_CALIBRATE;

#define SETTINGS									\
	SETTING(CONFIG_FLIGHT_NAME, STR, "TEST")		\
	SETTING(CONFIG_CALIBRATE_ACC, INT, 0)			\
	SETTING(CONFIG_CALIBRATE_GYR, INT, 0)			\
	SETTING(CONFIG_CALIBRATE_MAG, INT, 0)			\
	SETTING(CONFIG_TVC_X_INIT_ANGLE, FLT, 45.0f)	\
	SETTING(CONFIG_TVC_X_MAX_D_ANGLE, FLT, 21.0f)	\
	SETTING(CONFIG_TVC_X_SCALE_FAC, FLT, 4.17055f)	\
	SETTING(CONFIG_TVC_Y_INIT_ANGLE, FLT, 45.0f)	\
	SETTING(CONFIG_TVC_Y_MAX_D_ANGLE, FLT, 21.0f)	\
	SETTING(CONFIG_TVC_Y_SCALE_FAC, FLT, 4.17055f)	\

#define SETTING(__name, __type, __default_value) extern __type##_t __name;
SETTINGS
#undef SETTING

uint8_t config_load_settings();

#endif /* INC_CONFIG_H_ */
