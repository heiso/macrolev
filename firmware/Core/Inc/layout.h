/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LAYOUT_H
#define __LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define ADC_CHANNEL_COUNT 5
#define AMUX_CHANNEL_COUNT 16
#define MATRIX_ROWS 5
#define MATRIX_COLS 15
#define _BASE_LAYER 0
#define _TAP_LAYER 1

#define XXXX 0x00
#define ____ 0x00

const uint8_t row_col_by_channels[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT][2];
const uint16_t keymaps[2][MATRIX_ROWS][MATRIX_COLS];

#ifdef __cplusplus
}
#endif

#endif /* __LAYOUT_H */
