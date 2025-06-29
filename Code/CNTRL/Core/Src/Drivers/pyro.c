/*
 * pyro.c
 *
 *  Created on: Jun 29, 2025
 *      Author: aravs
 */

#include "Drivers/pyro.h"

void pyro_init(pyro* p) {
	HAL_GPIO_WritePin(p->pyro_port, p->pyro_pin, GPIO_PIN_RESET);
	p->state = PYRO_STANDBY;
	p->fire_time = 0;
}

void pyro_fire(pyro* p) {
	GPIO_PinState cont_state = HAL_GPIO_ReadPin(p->cont_port, p->cont_pin);

	if (p->state == PYRO_STANDBY && cont_state == GPIO_PIN_SET) {
		HAL_GPIO_WritePin(p->pyro_port, p->pyro_pin, GPIO_PIN_SET);
		p->state = PYRO_FIRING;
		p->fire_time = HAL_GetTick();
	} else if (p->state == PYRO_STANDBY) {
		p->state = PYRO_BROKEN;
	}
}

void pyro_update(pyro* p) {
	GPIO_PinState cont_state = HAL_GPIO_ReadPin(p->cont_port, p->cont_pin);

	if (p->state == PYRO_STANDBY) {
		if (cont_state == GPIO_PIN_RESET) p->state = PYRO_BROKEN;
		return;
	}

	if (p->state == PYRO_FIRING) {
		uint32_t now = HAL_GetTick();
		uint32_t elapsed = now - p->fire_time;

		if (cont_state == GPIO_PIN_RESET) {
			p->state = PYRO_CONFIRMED;
			HAL_GPIO_WritePin(p->pyro_port, p->pyro_pin, GPIO_PIN_RESET);
		} else if (elapsed >= p->fire_duration) {
			p->state = PYRO_TIMEOUT;
			HAL_GPIO_WritePin(p->pyro_port, p->pyro_pin, GPIO_PIN_RESET);
		}
	}
}
