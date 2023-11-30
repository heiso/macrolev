/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tusb.h"
#include "usb_descriptors.h"
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
// ADC_ChannelConfTypeDef sConfig;
ADC_ChannelConfTypeDef sConfig = {0};

// {adc_channel, amux_channel}
const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
    {{0, 10}, {0, 8}, {0, 7}, {0, 5}, {1, 9}, {1, 8}, {1, 6}, {2, 10}, {2, 9}, {2, 7}, {2, 6}, {3, 9}, {3, 8}, {3, 6}, {4, 2}},
    {{0, 11}, {0, 9}, {0, 6}, {0, 4}, {1, 10}, {1, 7}, {1, 5}, {2, 11}, {2, 8}, {2, 5}, {2, 4}, {3, 10}, {3, 7}, {3, 5}, {4, 1}},
    {{0, 12}, {0, 14}, {0, 2}, {1, 11}, {1, 14}, {1, 1}, {1, 4}, {2, 12}, {2, 15}, {2, 3}, {3, 11}, {3, 14}, {3, 1}, {3, 4}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 13}, {0, 0}, {0, 3}, {1, 13}, {1, 15}, {1, 2}, {1, 3}, {2, 14}, {2, 0}, {2, 2}, {3, 12}, {3, 15}, {3, 3}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 15}, {0, 1}, {1, 12}, {XXXX, XXXX}, {1, 0}, {XXXX, XXXX}, {2, 13}, {XXXX, XXXX}, {2, 1}, {3, 13}, {XXXX, XXXX}, {3, 0}, {3, 2}, {4, 0}},
};

// clang-format off
const uint16_t keymaps[2][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE_LAYER] = {
        {HID_KEY_ESCAPE, HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE},
        {____, HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_ENTER},
        {____, HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_EUROPE_1, XXXX},
        {XXXX, HID_KEY_SHIFT_LEFT, HID_KEY_F3, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, XXXX},
        {XXXX, HID_KEY_CONTROL_LEFT, HID_KEY_ALT_LEFT, HID_KEY_GUI_LEFT, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_GUI_RIGHT, HID_KEY_ALT_RIGHT, XXXX, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT},
    },
    [_TAP_LAYER] = {
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, XXXX},
        {XXXX, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, HID_KEY_ARROW_UP, XXXX},
        {XXXX, ____, ____, ____, XXXX, ____, XXXX, ____, XXXX, ____, ____, XXXX, ____, ____, ____},
    },
};
// clang-format on

const uint32_t adc_channels[ADC_CHANNEL_COUNT] = {ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7};
const uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

static struct key keys[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT];

static uint8_t should_send_report = 0;
static uint8_t can_send_report = 0;

static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {0, 0, 0, 0, 0, 0};
static uint8_t current_keycode_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */
static void update_key_state(struct key *key);
static void update_key(struct key *key);
static void keys_loop();
static void init_key(uint8_t adc_channel, uint8_t amux_channel, uint8_t row, uint8_t col, uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled);
static void keys_init(uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled);
static void keys_on_change(struct key *key);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(300);

  keys_init(25, 0, 0);

  tud_init(BOARD_TUD_RHPORT);
  can_send_report = 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    tud_task();

    keys_loop();

    if (should_send_report && can_send_report && tud_hid_ready()) {
      if (tud_suspended()) {
        tud_remote_wakeup();
      } else {
        can_send_report = 0;
        should_send_report = 0;
        tud_hid_keyboard_report(ITF_NUM_KEYBOARD, modifiers, keycodes);
      }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 13;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 5;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief USB_OTG_FS Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_OTG_FS_PCD_Init(void) {

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void update_key_state(struct key *key) {
  struct state state;

  // Get a reading
  state.value = HAL_ADC_GetValue(&hadc1);

  if (key->calibration.is_calibrating) {
    // Calibrate idle value
    key->calibration.idle_value = (1 - 0.6) * state.value + 0.6 * key->calibration.idle_value;

    key->calibration.cycles_count++;

    if (key->calibration.cycles_count >= CALIBRATION_CYCLES) {
      key->calibration.is_calibrating = 0;
    }
  }

  if (key->calibration.is_calibrating == 0) {
    // Calibrate idle value
    // if (state.value > key->calibration.idle_value) {
    //   key->calibration.idle_value = (1 - 0.8) * state.value + 0.8 * key->calibration.idle_value;
    // }

    // Get distance from top
    if (state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
      state.distance = 0;
    } else {
      state.distance = key->calibration.idle_value - state.value;
    }

    // Calibrate max distance value
    if (state.distance > key->calibration.max_distance) {
      // key->calibration.max_distance = (1 - 0.8) * state.distance + 0.8 * key->calibration.max_distance;
      key->calibration.max_distance = state.distance;
    }

    // Limit max distance
    if (state.distance >= key->calibration.max_distance - MAX_DISTANCE_OFFSET) {
      state.distance = key->calibration.max_distance;
    }

    // Map distance in percentages
    state.distance_percentage = (state.distance * 100) / key->calibration.max_distance;

    // Update velocity
    state.velocity = state.distance_percentage - key->state.distance_percentage;

    // Update Acceleration
    state.acceleration = (state.velocity - key->state.velocity) / 2;
    // state.acceleration = 0;

    // Update jerk
    state.jerk = (state.acceleration - key->state.acceleration) / 3;
    // state.jerk = 0;

    // Update movement
    state.is_pressing = key->state.velocity > 0 && state.velocity > 0;
  }

  key->state = state;
}

void update_key(struct key *key) {
  update_key_state(key);

  if (!key->calibration.is_calibrating) {
    /**
     * https://www.youtube.com/watch?v=_Sl-T6iQr8U&t
     *
     * |-----| <- FULL RESET      -
     * |     |                    | Continuous rapid trigger domain (deactivated when full_reset)
     * |     |                    |
     * |  -  | <- trigger_offset  -
     * |     |                    | Rapid trigger domain
     * |     |                    |
     * |-----|                    -
     *
     */

    uint32_t now = HAL_GetTick();
    uint8_t is_ready_to_trigger = key->actuation.triggered_at + MIN_TIME_BETWEEN_EVENTS < now;
    uint8_t has_moved_more_than_min = abs(key->state.distance_percentage - key->actuation.changed_at) > MIN_MOVEMENT_BETWEEN_EVENTS;
    uint8_t is_after_trigger_offset = key->state.distance_percentage > key->actuation.trigger_offset;

    switch (key->actuation.status) {

    case STATUS_RESET:
    case STATUS_RESET_AFTER_TAP:
      // if reset, can be triggered or tap
      if (has_moved_more_than_min && is_after_trigger_offset) {
        if (key->has_tap_layer) {
          key->actuation.status = STATUS_MIGHT_BE_TAP;
        } else if (is_ready_to_trigger) {
          key->actuation.status = STATUS_TRIGGERED;
          keys_on_change(key);
        }
        key->actuation.triggered_at = now;
        key->actuation.changed_at = key->state.distance_percentage;
      }
      break;

    case STATUS_MIGHT_BE_TAP:
      // if might be tap, can be tap or triggered
      if (has_moved_more_than_min && !is_after_trigger_offset && now - key->actuation.triggered_at <= TAP_TIMEOUT) {
        key->actuation.status = STATUS_TAP;
      } else if (now - key->actuation.triggered_at > TAP_TIMEOUT) {
        key->actuation.status = STATUS_TRIGGERED;
      }
      key->actuation.changed_at = key->state.distance_percentage;
      keys_on_change(key);
      break;

    case STATUS_TAP:
      // if tap, can be reset
      key->actuation.status = STATUS_RESET_AFTER_TAP;
      key->actuation.changed_at = key->state.distance_percentage;
      keys_on_change(key);
      break;

    case STATUS_TRIGGERED:
      // if triggered, can be reset
      if (has_moved_more_than_min && !is_after_trigger_offset) {
        key->actuation.status = STATUS_RESET;
        key->actuation.changed_at = key->state.distance_percentage;
        keys_on_change(key);
      }
      break;
    }

    // // Trigger
    // if (is_ready_to_trigger && key->actuation.is_reset && has_moved_more_than_min && is_after_trigger_offset) {
    //   key->actuation.is_reset = 0;
    //   key->actuation.triggered_at = now;
    //   key->actuation.changed_at = key->state.distance_percentage;
    //   // keys_on_triggered(key);
    //   keys_on_change(key);
    // }

    // // Reset
    // else if (!key->actuation.is_reset && has_moved_more_than_min && !is_after_trigger_offset) {
    //   key->actuation.changed_at = key->state.distance_percentage;
    //   key->actuation.is_reset = 1;
    //   // keys_on_reset(key);
    //   keys_on_change(key);
    // }

    // Full reset
    if (key->state.distance_percentage == 0) {
      key->actuation.changed_at = 0;
      if (key->actuation.status != STATUS_RESET && key->actuation.status != STATUS_RESET_AFTER_TAP) {
        key->actuation.status = STATUS_RESET;
        keys_on_change(key);
      }
    }
  }
}

void keys_loop() {
  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
      HAL_GPIO_WritePin(GPIOB, amux_select_pins[i], (amux_channel >> i) & 1);
    }

    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      // if (keys[adc_channel][amux_channel].is_enabled == 0) {
      //   // no hall effect sensor on this channel
      //   continue;
      // }

      // sConfig.Channel = adc_channels[adc_channel];
      // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);

      if (keys[adc_channel][amux_channel].is_enabled == 1) {
        update_key(&keys[adc_channel][amux_channel]);
      }
    }
    HAL_ADC_Stop(&hadc1);
  }

  // sConfig.Channel = adc_channels[0];
  // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  // HAL_ADC_Start(&hadc1);
  // HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

  // update_key(&keys[0][0]);

  // HAL_ADC_Stop(&hadc1);
};

void init_key(uint8_t adc_channel, uint8_t amux_channel, uint8_t row, uint8_t col, uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  struct key *key = &keys[adc_channel][amux_channel];

  key->is_enabled = 1;
  key->has_tap_layer = keymaps[_TAP_LAYER][row][col] != ____;
  key->modifier_bitmask = NO_MODIFIER_BITMASK;

  key->row = row;
  key->column = col;

  key->calibration.is_calibrating = 1;
  key->calibration.cycles_count = 0;
  key->calibration.idle_value = IDLE_VALUE_APPROX;
  key->calibration.max_distance = MAX_DISTANCE_APPROX;

  key->actuation.status = STATUS_RESET;
  key->actuation.changed_at = 0;
  key->actuation.trigger_offset = trigger_offset;
  key->actuation.rapid_trigger_offset = rapid_trigger_offset;
  key->actuation.is_continuous_rapid_trigger_enabled = is_continuous_rapid_trigger_enabled;
}

void keys_init(uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      struct key *key = &keys[adc_channel][amux_channel];
      key->is_enabled = 0;
    }
  }

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (channels_by_row_col[row][col][0] == XXXX || channels_by_row_col[row][col][1] == XXXX) {
        // no hall effect sensor on this channel
        continue;
      }

      init_key(channels_by_row_col[row][col][0], channels_by_row_col[row][col][1], row, col, trigger_offset, rapid_trigger_offset, is_continuous_rapid_trigger_enabled);
    }
  }
}

uint8_t get_bitmask_for_modifier(uint8_t keycode) {
  if (keycode == HID_KEY_CONTROL_LEFT) {
    return 0b00000001;
  } else if (keycode == HID_KEY_SHIFT_LEFT) {
    return 0b00000010;
  } else if (keycode == HID_KEY_ALT_LEFT) {
    return 0b00000100;
  } else if (keycode == HID_KEY_GUI_LEFT) {
    return 0b00001000;
  } else if (keycode == HID_KEY_CONTROL_RIGHT) {
    return 0b00010000;
  } else if (keycode == HID_KEY_SHIFT_RIGHT) {
    return 0b00100000;
  } else if (keycode == HID_KEY_ALT_RIGHT) {
    return 0b01000000;
  } else if (keycode == HID_KEY_GUI_RIGHT) {
    return 0b10000000;
  } else {
    return NO_MODIFIER_BITMASK;
  }
}

void keys_on_change(struct key *key) {
  if (keymaps[_BASE_LAYER][key->row][key->column] == XXXX) {
    return;
  }

  switch (key->actuation.status) {
  case STATUS_TRIGGERED:
    key->modifier_bitmask = get_bitmask_for_modifier(keymaps[_BASE_LAYER][key->row][key->column]);
    if (key->modifier_bitmask != NO_MODIFIER_BITMASK) {
      modifiers |= key->modifier_bitmask;
      should_send_report = 1;
    } else if (current_keycode_index < 6) {
      keycodes[current_keycode_index] = keymaps[_BASE_LAYER][key->row][key->column];
      current_keycode_index++;
      should_send_report = 1;
    }
    break;

  case STATUS_TAP:
    key->modifier_bitmask = get_bitmask_for_modifier(keymaps[_TAP_LAYER][key->row][key->column]);
    if (key->modifier_bitmask == NO_MODIFIER_BITMASK && current_keycode_index < 6) {
      keycodes[current_keycode_index] = keymaps[_TAP_LAYER][key->row][key->column];
      current_keycode_index++;
      should_send_report = 1;
    }
    break;

  case STATUS_RESET_AFTER_TAP:
    if (key->modifier_bitmask == NO_MODIFIER_BITMASK) {
      uint8_t new_keycodes[6] = {0, 0, 0, 0, 0, 0};
      uint8_t new_keycode_index = 0;

      for (uint8_t i = 0; i < 6; i++) {
        if (keycodes[i] != 0 && keycodes[i] != keymaps[_TAP_LAYER][key->row][key->column]) {
          new_keycodes[new_keycode_index] = keycodes[i];
          new_keycode_index++;
        }
      }

      for (uint8_t i = 0; i < 6; i++) {
        keycodes[i] = new_keycodes[i];
      }

      current_keycode_index = new_keycode_index;
      should_send_report = 1;
    }
    break;

  case STATUS_RESET:
    if (key->modifier_bitmask != NO_MODIFIER_BITMASK) {
      modifiers &= ~key->modifier_bitmask;
      key->modifier_bitmask = NO_MODIFIER_BITMASK;
    } else {
      uint8_t new_keycodes[6] = {0, 0, 0, 0, 0, 0};
      uint8_t new_keycode_index = 0;

      for (uint8_t i = 0; i < 6; i++) {
        if (keycodes[i] != 0 && keycodes[i] != keymaps[_BASE_LAYER][key->row][key->column]) {
          new_keycodes[new_keycode_index] = keycodes[i];
          new_keycode_index++;
        }
      }

      for (uint8_t i = 0; i < 6; i++) {
        keycodes[i] = new_keycodes[i];
      }

      current_keycode_index = new_keycode_index;
    }
    should_send_report = 1;
    break;
  }
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
  (void)instance;
  (void)protocol;

  // nothing to do since we use the same compatible boot report for both Boot and Report mode.
  // TODO set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len) {
  (void)instance;
  (void)report;
  (void)len;

  can_send_report = 1;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
