/*
 * logger.h
 *
 *  Created on: Jun 27, 2025
 *      Author: aravs
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include "main.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "util.h"

FRESULT logger_sd_init();
FRESULT logger_sd_open_logfile();
FRESULT logger_sd_log_data(rocket_data* data);
FRESULT logger_sd_deinit();

#endif /* INC_LOGGER_H_ */
