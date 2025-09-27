#ifndef UTIL_H
#define UTIL_H

typedef enum {
	PYRO_STANDBY 	= 0,
	PYRO_FIRING	 	= 1,
	PYRO_CONFIRMED	= 2,
	PYRO_TIMEOUT	= 3,
	PYRO_BROKEN 	= 4
} pyro_state;

typedef struct {
	uint8_t calibrated	: 1;
	uint8_t armed		: 1;
	uint8_t ignition	: 1;
	uint8_t apogee		: 1;
	uint8_t touchdown	: 1;
} __attribute__((packed)) flight_event_flags;

typedef enum {
	FLIGHT_STATE_PAD,
	FLIGHT_STATE_ARMED,
	FLIGHT_STATE_BOOST,
	FLIGHT_STATE_DESCENT,
	FLIGHT_STATE_TOUCHDOWN,
	FLIGHT_STATE_LOGGING,
	FLIGHT_STATE_READY
} flight_state;

typedef struct {
	float T_plus;

	flight_event_flags flags;
	uint8_t state;

	float vbat;

	struct {
		float x;
		float y;
		float z;
	} acc;

	struct {
		float x;
		float y;
		float z;
	} gyr;

	struct {
		float x;
		float y;
		float z;
	} mag;

	struct {
		float w;
		float x;
		float y;
		float z;
	} quat;

	float temperature;
	float pressure;
	float altitude;
	float v_velocity;
	float v_accel;

	struct {
		float x;
		float y;
	} tvc;

	struct {
		uint8_t motor;
		uint8_t parachute;
	} pyro;
} __attribute__((packed)) rocket_data;

#endif // UTIL_H
