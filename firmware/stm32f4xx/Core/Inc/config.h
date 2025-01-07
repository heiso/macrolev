#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>

#define DEFAULT_REVERSE_MAGNET_POLE 0
#define DEFAULT_TRIGGER_OFFSET 64
#define DEFAULT_RESET_THRESHOLD 3
#define DEFAULT_RAPID_TRIGGER_OFFSET 40
#define DEFAULT_SCREAMING_VELOCITY_TRIGGER 45
#define DEFAULT_TAP_TIMEOUT 200

#define ADC_CHANNEL_COUNT 5
#define AMUX_SELECT_PINS_COUNT 4
#define AMUX_CHANNEL_COUNT 16

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

extern const struct user_config keyboard_default_user_config;
extern const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2];

#endif /* __CONFIG_H */
