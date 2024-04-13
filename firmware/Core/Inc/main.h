/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define CONFIG_ADDRESS 0x08040000

#define CALIBRATION_CYCLES 20

#define DEFAULT_TRIGGER_OFFSET 64
#define DEFAULT_RESET_THRESHOLD 3
#define DEFAULT_RAPID_TRIGGER_OFFSET 40

#define IDLE_VALUE_APPROX 1800
#define MAX_DISTANCE_APPROX 500
#define IDLE_VALUE_OFFSET 10
#define MAX_DISTANCE_OFFSET 60
#define TAP_TIMEOUT 200
#define IDLE_CYCLES_UNTIL_SLEEP 15

#define ADC_CHANNEL_COUNT 5
#define AMUX_SELECT_PINS_COUNT 4
#define AMUX_CHANNEL_COUNT 16

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define XXXX 0xff
#define ____ 0x00

#define SPECIAL(X) (0b1000000000000000 | X)
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct calibration {
  uint16_t cycles_count;
  uint16_t idle_value;
  uint16_t max_distance;
};

enum direction {
  GOING_UP,
  GOING_DOWN,
};

struct state {
  uint16_t value;
  uint16_t distance;
  uint8_t distance_8bits;
  float filtered_distance;
  int8_t velocity;
  uint8_t filtered_distance_8bits;
};

enum actuation_status {
  STATUS_MIGHT_BE_TAP,
  STATUS_TAP,
  STATUS_TRIGGERED,
  STATUS_RESET,
  STATUS_RAPID_TRIGGER_RESET
};

struct actuation {
  enum direction direction;
  uint8_t direction_changed_point;
  enum actuation_status status;
  uint8_t reset_offset;
  uint8_t trigger_offset;
  uint8_t rapid_trigger_offset;
  uint8_t is_continuous_rapid_trigger_enabled;
  uint32_t triggered_at;
};

enum key_type {
  KEY_TYPE_EMPTY,
  KEY_TYPE_NORMAL,
  KEY_TYPE_MODIFIER,
  KEY_TYPE_CONSUMER_CONTROL,
};

struct layer {
  enum key_type type;
  uint16_t value;
};

enum {
  _BASE_LAYER,
  _TAP_LAYER,
  LAYERS_COUNT
};

struct key {
  uint8_t is_enabled;
  uint8_t row;
  uint8_t column;
  uint8_t idle_counter;
  uint8_t is_idle;
  struct layer layers[LAYERS_COUNT];
  struct calibration calibration;
  struct state state;
  struct actuation actuation;
};

struct serial_key {
  uint8_t row;
  uint8_t column;
  uint16_t idle_value;
  uint16_t max_distance;
  uint16_t value;
  uint8_t distance_8bits;
  enum actuation_status status;
};

struct user_config {
  uint8_t trigger_offset;
  uint8_t reset_threshold;
  uint8_t rapid_trigger_offset;
  uint16_t keymaps[LAYERS_COUNT][MATRIX_ROWS][MATRIX_COLS];
};
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
