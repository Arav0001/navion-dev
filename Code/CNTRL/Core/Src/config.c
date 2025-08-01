/*
 * config.c
 *
 *  Created on: Jul 31, 2025
 *      Author: aravs
 */

#include "config.h"

#include <stdlib.h>
#include <ctype.h>

#include "logger.h"

uint8_t CONFIG_CALIBRATE = 0;

#define SETTING(__name, __type, __default_value) __type##_t __name = __default_value;
SETTINGS
#undef SETTING

FIL settings_file;

typedef enum {
	TOKEN_TYPE_INT,
	TOKEN_TYPE_FLT,
	TOKEN_TYPE_STR,
	TOKEN_TYPE_INVALID,
} token_type;

typedef struct {
	char key[TOKEN_STRING_SIZE];
	token_type type;

	union {
		int32_t INT_val;
		float FLT_val;
		char STR_val[TOKEN_STRING_SIZE];
	} value;
} token;

static char* trim_spaces(char* str) {
	while (isspace((unsigned char) *str)) str++;
	if (*str == 0) return str;

	char* end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char) *end)) end--;

	*(end + 1) = '\0';

	return str;
}

uint8_t parse_token_line(const char* input, token *output) {
	if (!input || !output) return 0;

	char line[MAX_LINE_LENGTH];
	strncpy(line, input, MAX_LINE_LENGTH);
	line[MAX_LINE_LENGTH - 1] = '\0';

	char* comment = strpbrk(line, "#");
	if (comment) *comment = '\0';

	char* equal_sign = strchr(line, '=');
	if (!equal_sign) return 0;

	*equal_sign = '\0';
	char* key = trim_spaces(line);
	char* value = trim_spaces(equal_sign + 1);

	if (strlen(key) == 0 || strlen(value) == 0) return 0;

	strncpy(output->key, key, TOKEN_STRING_SIZE);
	output->key[TOKEN_STRING_SIZE - 1] = '\0';

	// strings
	if (value[0] == '"' && value[strlen(value) - 1] == '"') {
		value[strlen(value) - 1] = '\0';
		value++;

		output->type = TOKEN_TYPE_STR;

		strncpy(output->value.STR_val, value, TOKEN_STRING_SIZE);
		output->value.STR_val[TOKEN_STRING_SIZE - 1] = '\0';

		return 1;
	}

	// bools
	if (strcasecmp(value, "true") == 0) {
		output->type = TOKEN_TYPE_INT;
		output->value.INT_val = 1;

		return 1;
	} else if (strcasecmp(value, "false") == 0) {
		output->type = TOKEN_TYPE_INT;
		output->value.INT_val = 0;

		return 1;
	}

	// floats
	if (strchr(value, '.')) {
		char* end;
		float val = strtof(value, &end);
		if (*end != '\0') return 0;

		output->type = TOKEN_TYPE_FLT;
		output->value.FLT_val = val;

		return 1;
	}

	// ints
	char* end;
	long val = strtol(value, &end, 10);
	if (*end != '\0') return 0;

	output->type = TOKEN_TYPE_INT;
	output->value.INT_val = (INT_t)val;

	return 1;
}

uint8_t config_override_settings(const token* tok) {
	if (!tok || strlen(tok->key) == 0) return 0;

	uint8_t is_str_type;

	#define SETTING(__name, __type, __default_value)			\
		is_str_type = strcmp(#__type, "STR") == 0;				\
		if (strcmp(tok->key, #__name + 7) == 0 &&				\
			((tok->type == TOKEN_TYPE_##__type) ||				\
			(tok->type == TOKEN_TYPE_STR && is_str_type))) {	\
			memcpy(&__name, &tok->value, sizeof(__name));		\
			if (is_str_type) {									\
                ((char*)&__name)[TOKEN_STRING_SIZE - 1] = '\0';	\
            }													\
            return 1;											\
        }

    SETTINGS
    #undef SETTING

	return 0;
}

uint8_t config_load_settings() {
	FRESULT res;
	char line[MAX_LINE_LENGTH];

	if (mount_sd() != FR_OK) return 0;

	res = f_open(&settings_file, SETTINGS_FILE, FA_READ);
	if (res != FR_OK) return 0;

	while (f_gets(line, MAX_LINE_LENGTH, &settings_file)) {
		token tok;
		if (parse_token_line(line, &tok)) {
			config_override_settings(&tok);
		}
	}

	f_close(&settings_file);

	CONFIG_CALIBRATE = CONFIG_CALIBRATE_ACC || CONFIG_CALIBRATE_GYR || CONFIG_CALIBRATE_MAG;

	return 1;
}
