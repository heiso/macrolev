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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct calibration {
  uint16_t cycles_count;
  uint16_t idle_value;
  uint16_t max_distance;
};

struct state {
  uint16_t value;
  uint16_t distance;
  uint16_t distance_percentage;
  int16_t velocity;
  int16_t acceleration;
  int16_t jerk;
};

enum actuation_status {
  STATUS_RESET,
  STATUS_MIGHT_BE_TAP,
  STATUS_TAP,
  STATUS_TRIGGERED,
};

struct actuation {
  enum actuation_status status;
  uint16_t changed_at;
  uint16_t reset_offset;
  uint16_t trigger_offset;
  uint16_t rapid_trigger_offset;
  uint8_t is_continuous_rapid_trigger_enabled;
  uint32_t triggered_at;
};

struct layer {
  uint8_t keycode;
  uint8_t modifier_bitmask;
};

struct key {
  uint8_t is_enabled;
  uint8_t idle_counter;
  uint8_t row;
  uint8_t column;
  struct layer layers[2];
  struct calibration calibration;
  struct state state;
  struct actuation actuation;
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
#define CALIBRATION_CYCLES 20

#define TRIGGER_OFFSET 25
#define MIN_DISTANCE_BETWEEN_TRIGGER_AND_RESET 3

#define IDLE_VALUE_APPROX 1800
#define MAX_DISTANCE_APPROX 500
#define IDLE_VALUE_OFFSET 10
#define MAX_DISTANCE_OFFSET 40
#define TAP_TIMEOUT 90
#define IDLE_CYCLES_UNTIL_SLEEP 15

#define ADC_CHANNEL_COUNT 5
#define AMUX_SELECT_PINS_COUNT 4
#define AMUX_CHANNEL_COUNT 16

#define MATRIX_ROWS 5
#define MATRIX_COLS 15
#define _BASE_LAYER 0
#define _TAP_LAYER 1

#define XXXX 0xff
#define ____ 0x00
#define NO_KEYCODE ____
#define NO_MODIFIER_BITMASK 0b00000000

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
