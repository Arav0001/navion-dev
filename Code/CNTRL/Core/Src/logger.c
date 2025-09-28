/*
 * logger.c
 *
 *  Created on: Jun 27, 2025
 *      Author: aravs
 */

#include "logger.h"

/* FLASH */
#define FLASH_PAGE_SIZE 256
#define FLASH_LOG_SIZE sizeof(rocket_data)
#define FLASH_NUM_PAGES 65536

extern SPI_HandleTypeDef hspi2;
#define FLASH_SPI &hspi2
#define FLASH_CS_GPIO_PORT SPI2_CSB_GPIO_Port
#define FLASH_CS_GPIO_PIN SPI2_CSB_Pin

W25QXX_HandleTypeDef flash;
W25QXX_result_t flash_result;

uint32_t flash_page_idx = 0;
/* FLASH */

/* SD CARD */
#define LOGGER_CSV_ROW_SIZE 256
#define LOG_FILENAME_SUFFIX "_DATA.csv"
#define LOG_FILENAME_MAX_SIZE 64

char logfile_name[LOG_FILENAME_MAX_SIZE];
char csv_row[LOGGER_CSV_ROW_SIZE];
const char* csv_header = "T+,calib?,armed?,ignition?,apogee?,touchdown?,state,Vbat,ax,ay,az,gx,gy,gz,mx,my,mz,qw,qx,qy,qz,temp,pres,alt,vvel,vaccel,tvc_x,tvc_y,pyro_motor,pyro_parachute\r\n";

FATFS SD_FatFs;
FIL logfile;
FRESULT sd_result;

void build_log_filename(const char* flight_name) {
	size_t suffix_len = strlen(LOG_FILENAME_SUFFIX);
	size_t flight_name_len = strlen(flight_name);

	size_t max_flight_name_len = LOG_FILENAME_MAX_SIZE - suffix_len - 1;

	if (flight_name_len > max_flight_name_len) {
		flight_name_len = max_flight_name_len;
	}

	memcpy(logfile_name, flight_name, flight_name_len);
	strcpy(logfile_name + flight_name_len, LOG_FILENAME_SUFFIX);
}
/* SD CARD */

/* FLASH LOGGER */
W25QXX_result_t logger_flash_init() {
	_Static_assert(FLASH_LOG_SIZE < FLASH_PAGE_SIZE, "rocket_data too large");

	HAL_Delay(10);

	flash_result = w25qxx_init(&flash, FLASH_SPI, FLASH_CS_GPIO_PORT, FLASH_CS_GPIO_PIN);

	w25qxx_chip_erase(&flash);

	return flash_result;
}

W25QXX_result_t logger_flash_log_data(rocket_data* data) {
	uint8_t log_buffer[FLASH_LOG_SIZE];

	memcpy(log_buffer, data, FLASH_LOG_SIZE);

	if (flash_page_idx <= FLASH_NUM_PAGES) {
		flash_result = w25qxx_write(&flash, flash_page_idx * FLASH_PAGE_SIZE, log_buffer, FLASH_LOG_SIZE);
	} else {
		flash_result = W25QXX_Err;
	}

	if (flash_result != W25QXX_Ok) {
		return flash_result;
	}

	flash_page_idx++;

	return flash_result;
}

W25QXX_result_t logger_flash_read_data(uint32_t idx, rocket_data* data) {
	uint8_t log_buffer[FLASH_LOG_SIZE];

	if (idx > FLASH_NUM_PAGES) return W25QXX_Err;

	flash_result = w25qxx_read(&flash, idx * FLASH_PAGE_SIZE, log_buffer, FLASH_LOG_SIZE);
	if (flash_result != W25QXX_Ok) return flash_result;

	memcpy(data, log_buffer, FLASH_LOG_SIZE);

	return flash_result;
}
/* FLASH LOGGER */

/* SD LOGGER */
FRESULT mount_sd() {
	sd_result = f_mount(&SD_FatFs, "", 1);
	return sd_result;
}

FRESULT logger_sd_init() {
	build_log_filename(CONFIG_FLIGHT_NAME);

	sd_result = f_open(&logfile, logfile_name, FA_WRITE | FA_OPEN_ALWAYS);
//	if (log_result != FR_OK) return log_result;
//
//	log_result = f_lseek(&logfile, f_size(&logfile));
//	if (log_result != FR_OK) f_close(&logfile);

	UINT bytes_written;
	sd_result = f_write(&logfile, csv_header, strlen(csv_header), &bytes_written);
	if (sd_result != FR_OK) return sd_result;
	if (bytes_written != strlen(csv_header)) return FR_DISK_ERR;

	return FR_OK;
}

FRESULT logger_sd_log_data(rocket_data* data) {
	UINT bytes_written;

	int len = snprintf(csv_row, LOGGER_CSV_ROW_SIZE,
		"%.3f,"               		// T_plus
		"%d,%d,%d,%d,%d,"			// flags
		"%s,"						// flight state
		"%.3f,"						// Vbat
		"%.3f,%.3f,%.3f,"     		// acc
		"%.3f,%.3f,%.3f,"     		// gyr
		"%.3f,%.3f,%.3f,"     		// mag
		"%.3f,%.3f,%.3f,%.3f,"		// quat
		"%.3f,%.3f,%.3f,%.3f,%.3f,"	// barometer
		"%.3f,%.3f,"          		// tvc
		"%s,%s\r\n",          		// pyro states
		data->T_plus,
		data->flags.calibrated, data->flags.armed, data->flags.ignition, data->flags.apogee, data->flags.touchdown,
		flight_state_to_str(data->state),
		data->vbat,
		data->acc.x, data->acc.y, data->acc.z,
		data->gyr.x, data->gyr.y, data->gyr.z,
		data->mag.x, data->mag.y, data->mag.z,
		data->quat.w, data->quat.x, data->quat.y, data->quat.z,
		data->temperature, data->pressure, data->altitude, data->v_velocity, data->v_accel,
		data->tvc.x, data->tvc.y,
		pyro_state_to_str(data->pyro.motor),
		pyro_state_to_str(data->pyro.parachute)
	);

	if (len < 0 || len >= LOGGER_CSV_ROW_SIZE) return FR_INT_ERR;

	sd_result = f_write(&logfile, csv_row, len, &bytes_written);

	if (sd_result != FR_OK) return sd_result;
	if (bytes_written != len) return FR_DISK_ERR;

	return FR_OK;
}

FRESULT logger_sd_deinit() {
	sd_result = f_sync(&logfile);
	if (sd_result != FR_OK) return sd_result;

	sd_result = f_close(&logfile);
	return sd_result;
}
/* SD LOGGER */

void logger_copy_flash_to_sd() {
	rocket_data buffer = {0};

	logger_sd_init();

	for (uint32_t idx = 0; idx < flash_page_idx; idx++) {
		logger_flash_read_data(idx, &buffer);
		logger_sd_log_data(&buffer);
	}

	logger_sd_deinit();
}
