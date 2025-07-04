/*
 * logger.c
 *
 *  Created on: Jun 27, 2025
 *      Author: aravs
 */

#include "logger.h"

#define LOGGER_CSV_ROW_SIZE 256

const char* logfile_name = FLIGHT_NAME "_DATA.csv";
char csv_row[LOGGER_CSV_ROW_SIZE];

const char* csv_header = "T+,ax,ay,az,gx,gy,gz,mx,my,mz,qw,qx,qy,qz,tvc_x,tvc_y,pyro_motor,pyro_parachute\r\n";

FATFS SD_FatFs;
FIL logfile;
FRESULT log_result;

FRESULT logger_sd_init() {
	log_result = f_mount(&SD_FatFs, "", 1);
	return log_result;
}

FRESULT logger_sd_open_logfile() {
	log_result = f_open(&logfile, logfile_name, FA_WRITE | FA_OPEN_ALWAYS | FA_OPEN_APPEND);
//	if (log_result != FR_OK) return log_result;
//
//	log_result = f_lseek(&logfile, f_size(&logfile));
//	if (log_result != FR_OK) f_close(&logfile);

	if (f_size(&logfile) == 0) {
		UINT bytes_written;
		log_result = f_write(&logfile, csv_header, strlen(csv_header), &bytes_written);
		if (log_result != FR_OK) return log_result;
		if (bytes_written != strlen(csv_header)) return FR_DISK_ERR;
	}

	return FR_OK;
}

FRESULT logger_sd_log_data(rocket_data* data) {
	UINT bytes_written;

	int len = snprintf(csv_row, LOGGER_CSV_ROW_SIZE,
		"%.3f,"               	// T_plus
		"%.3f,%.3f,%.3f,"     	// acc
		"%.3f,%.3f,%.3f,"     	// gyr
		"%.3f,%.3f,%.3f,"     	// mag
		"%.3f,%.3f,%.3f,%.3f,"	// quat
		"%.3f,%.3f,"          	// tvc
		"%s,%s\r\n",          	// pyro states
		data->T_plus,
		data->acc.x, data->acc.y, data->acc.z,
		data->gyr.x, data->gyr.y, data->gyr.z,
		data->mag.x, data->mag.y, data->mag.z,
		data->quat.w, data->quat.x, data->quat.y, data->quat.z,
		data->tvc.x, data->tvc.y,
		pyro_state_to_str(data->pyro.motor),
		pyro_state_to_str(data->pyro.parachute)
	);

	if (len < 0 || len >= LOGGER_CSV_ROW_SIZE) return FR_INT_ERR;

	log_result = f_write(&logfile, csv_row, len, &bytes_written);

	if (log_result != FR_OK) return log_result;
	if (bytes_written != len) return FR_DISK_ERR;

	return FR_OK;
}

FRESULT logger_sd_deinit() {
	log_result = f_sync(&logfile);
	if (log_result != FR_OK) return log_result;

	log_result = f_close(&logfile);
	return log_result;
}
