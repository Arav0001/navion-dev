/*
 * pyro.h
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_PYRO_H_
#define INC_DRIVERS_PYRO_H_

#include "main.h"

typedef enum {
	PYRO_STANDBY 	= 0,
	PYRO_FIRING	 	= 1,
	PYRO_CONFIRMED	= 3,
	PYRO_TIMEOUT	= 4,
	PYRO_BROKEN 	= 5
} pyro_state;

typedef struct {
	GPIO_TypeDef* pyro_port;
	uint16_t pyro_pin;

	GPIO_TypeDef* cont_port;
	uint16_t cont_pin;

	pyro_state state;

	const uint32_t fire_duration;	// milliseconds
	uint32_t fire_time;			// milliseconds
} pyro;

void pyro_init(pyro* p);
void pyro_fire(pyro* p);
void pyro_update(pyro* p);

#endif /* INC_DRIVERS_PYRO_H_ */
