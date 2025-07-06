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

#include "Drivers/w25qxx.h"

#include "config.h"
#include "util.h"

/* FLASH LOGGER */
W25QXX_result_t logger_flash_init();
W25QXX_result_t logger_flash_log_data(rocket_data* data);
W25QXX_result_t logger_flash_read_data(uint32_t idx, rocket_data* data);
/* FLASH LOGGER */

/* SD LOGGER */
FRESULT logger_sd_init();
FRESULT logger_sd_open_logfile();
FRESULT logger_sd_log_data(rocket_data* data);
FRESULT logger_sd_deinit();
/* SD LOGGER */

void logger_copy_to_sd(uint32_t entries);

#endif /* INC_LOGGER_H_ */
