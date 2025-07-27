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
	float T_plus;

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

	struct {
		float x;
		float y;
	} tvc;

	struct {
		pyro_state motor;
		pyro_state parachute;
	} pyro;
} __attribute__((packed)) rocket_data;

#endif // UTIL_H
