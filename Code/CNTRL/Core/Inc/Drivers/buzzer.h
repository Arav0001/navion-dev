/*
 * buzzer.h
 *
 *  Created on: Jul 20, 2025
 *      Author: aravs
 */

#ifndef INC_DRIVERS_BUZZER_H_
#define INC_DRIVERS_BUZZER_H_

#include "main.h"
#include "Drivers/pwm.h"

typedef struct {
	pwm_channel pwm;
} buzzer;

void buzzer_init(buzzer* bzr);
void buzzer_set_volume(buzzer* bzr, float volume);
void buzzer_stop(buzzer* bzr);

#endif /* INC_DRIVERS_BUZZER_H_ */
