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
#include "DRV2605L.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define IS_DRV2605_ENABLED false

#define VENDOR_REQUEST_KEYS 0xfe
#define VENDOR_REQUEST_CONFIG 0xff
#define VENDOR_REQUEST_RESET_CONFIG 0xfd

#define VENDOR_VALUE_GET_LENGTH 0x00
#define VENDOR_VALUE_GET 0x01
#define VENDOR_VALUE_SET 0x02

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
ADC_ChannelConfTypeDef ADC_channel_Config = {0};

CFG_TUSB_MEM_SECTION CFG_TUSB_MEM_ALIGN static uint8_t usb_vendor_control_buffer[CFG_TUD_VENDOR_RX_BUFSIZE];

struct user_config user_config = {0};

const struct user_config default_user_config = {
    .trigger_offset = DEFAULT_TRIGGER_OFFSET,
    .reset_threshold = DEFAULT_RESET_THRESHOLD,
    .rapid_trigger_offset = DEFAULT_RAPID_TRIGGER_OFFSET,
    .keymaps = {
        // clang-format off
        [_BASE_LAYER] = {
            {HID_KEY_ESCAPE, HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE},
            {SPECIAL(HID_USAGE_CONSUMER_VOLUME_INCREMENT), HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_ENTER},
            {SPECIAL(HID_USAGE_CONSUMER_VOLUME_DECREMENT), HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_EUROPE_1, XXXX},
            {XXXX, HID_KEY_SHIFT_LEFT, HID_KEY_EUROPE_2, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, XXXX},
            {XXXX, HID_KEY_CONTROL_LEFT, HID_KEY_ALT_LEFT, HID_KEY_GUI_LEFT, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_GUI_RIGHT, HID_KEY_ALT_RIGHT, XXXX, HID_KEY_ARROW_LEFT, SPECIAL(HID_USAGE_CONSUMER_PLAY_PAUSE), HID_KEY_ARROW_RIGHT},
        },
        [_TAP_LAYER] = {
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, XXXX},
            {XXXX, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, HID_KEY_ARROW_UP, XXXX},
            {XXXX, ____, ____, ____, XXXX, ____, XXXX, ____, XXXX, ____, ____, XXXX, ____, HID_KEY_ARROW_DOWN, ____},
        },
        // clang-format on
    }};

// {adc_channel, amux_channel}
const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
    {{0, 10}, {0, 8}, {0, 7}, {0, 5}, {1, 9}, {1, 8}, {1, 6}, {2, 10}, {2, 9}, {2, 7}, {2, 6}, {3, 9}, {3, 8}, {3, 6}, {4, 2}},
    {{0, 11}, {0, 9}, {0, 6}, {0, 4}, {1, 10}, {1, 7}, {1, 5}, {2, 11}, {2, 8}, {2, 5}, {2, 4}, {3, 10}, {3, 7}, {3, 5}, {4, 1}},
    {{0, 12}, {0, 14}, {0, 2}, {1, 11}, {1, 14}, {1, 1}, {1, 4}, {2, 12}, {2, 15}, {2, 3}, {3, 11}, {3, 14}, {3, 1}, {3, 4}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 13}, {0, 0}, {0, 3}, {1, 13}, {1, 15}, {1, 2}, {1, 3}, {2, 14}, {2, 0}, {2, 2}, {3, 12}, {3, 15}, {3, 3}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 15}, {0, 1}, {1, 12}, {XXXX, XXXX}, {1, 0}, {XXXX, XXXX}, {2, 13}, {XXXX, XXXX}, {2, 1}, {3, 13}, {XXXX, XXXX}, {3, 0}, {3, 2}, {4, 0}},
};

const uint32_t adc_channels[ADC_CHANNEL_COUNT] = {ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7};
const uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

static struct key keys[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT] = {0};

static uint8_t key_triggered = 0;

static uint8_t should_send_consumer_report = 0;
static uint8_t should_send_keyboard_report = 0;

static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {0};
static uint8_t consumer_report = 0;

extern uint8_t const desc_ms_os_20[];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
#if IS_DRV2605_ENABLED
static void MX_I2C1_Init(void);
#endif
/* USER CODE BEGIN PFP */
static void update_key(struct key *key);
static void init_keys();
static void add_to_hid_report(struct key *key, uint8_t layer);
static void remove_from_hid_report(struct key *key, uint8_t layer);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void readConfig() {
  memcpy(&user_config, (uint32_t *)CONFIG_ADDRESS, sizeof(user_config));
}

bool writeConfig(uint8_t *buffer, uint16_t offset, uint16_t size) {
  if (offset >= sizeof user_config) {
    return false;
  }

  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
  FLASH_Erase_Sector(FLASH_SECTOR_6, VOLTAGE_RANGE_3);
  for (uint16_t i = offset; i < size; i++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, CONFIG_ADDRESS + i, buffer[i]) != HAL_OK) {
      HAL_FLASH_Lock();
    };
  }
  HAL_FLASH_Lock();
  return true;
}

uint8_t readRegister8(uint8_t reg) {
  uint8_t buffer[1] = {reg};
  HAL_I2C_Master_Transmit(&hi2c1, DRV2605_ADDR << 1, buffer, 1, 1000);
  HAL_I2C_Master_Receive(&hi2c1, DRV2605_ADDR << 1, buffer, 1, 1000);
  return buffer[0];
}

void writeRegister8(uint8_t reg, uint8_t val) {
  uint8_t buffer[2] = {reg, val};
  HAL_I2C_Master_Transmit(&hi2c1, DRV2605_ADDR << 1, buffer, sizeof(buffer), 1);
}

void DRV2605_init() {
  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby

  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback

  writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
  writeRegister8(DRV2605_REG_WAVESEQ2, 0); // end sequence

  writeRegister8(DRV2605_REG_OVERDRIVE, 1); // no overdrive

  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);

  // ERM open loop

  // turn on N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK,
                 readRegister8(DRV2605_REG_FEEDBACK) | 0x80);

  // // turn off N_ERM_LRA
  // writeRegister8(DRV2605_REG_FEEDBACK,
  //                readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // // turn on ERM_OPEN_LOOP
  // writeRegister8(DRV2605_REG_CONTROL3,
  //                readRegister8(DRV2605_REG_CONTROL3) | 0x20);

  writeRegister8(DRV2605_REG_LIBRARY, 1);
  writeRegister8(DRV2605_REG_MODE, DRV2605_MODE_INTTRIG);
}
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
#if IS_DRV2605_ENABLED
  MX_I2C1_Init();
#endif
  /* USER CODE BEGIN 2 */

  init_keys();

  tud_init(BOARD_TUD_RHPORT);
#if IS_DRV2605_ENABLED
  drv2605l_init();
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    // MARK: Main loop
    tud_task();

    key_triggered = 0;

    for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
      // TODO: set GPIOs at the same time using bitmap on register
      for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
        HAL_GPIO_WritePin(GPIOB, amux_select_pins[i], (amux_channel >> i) & 1);
      }

      for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
        if (keys[adc_channel][amux_channel].is_enabled == 0) {
          continue;
        }

        ADC_channel_Config.Channel = adc_channels[adc_channel];
        HAL_ADC_ConfigChannel(&hadc1, &ADC_channel_Config);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);

        update_key(&keys[adc_channel][amux_channel]);

        HAL_ADC_Stop(&hadc1);
      }
    }

    // If a key might be tap and a non tap key has been triggered, then the might be tap key is a normal trigger
    for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
      for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
        if (keys[adc_channel][amux_channel].is_enabled == 0 || keys[adc_channel][amux_channel].actuation.status != STATUS_MIGHT_BE_TAP) {
          continue;
        }

        struct key *key = &keys[adc_channel][amux_channel];
        uint8_t is_before_reset_offset = key->state.distance_8bits < key->actuation.reset_offset;
        uint8_t is_before_timeout = HAL_GetTick() - key->actuation.triggered_at <= TAP_TIMEOUT;

        // if might be tap, can be tap or triggered
        if (is_before_reset_offset && is_before_timeout) {
          key->actuation.status = STATUS_TAP;
          add_to_hid_report(key, _TAP_LAYER);
        } else if (!is_before_timeout || key_triggered) {
          key->actuation.status = STATUS_TRIGGERED;
          add_to_hid_report(key, _BASE_LAYER);
        }
      }
    }

    if ((should_send_consumer_report || should_send_keyboard_report) && tud_hid_ready()) {
      if (tud_suspended()) {
        tud_remote_wakeup();
      } else {
        if (should_send_consumer_report) {
          should_send_consumer_report = 0;
          tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_report, 2);
        } else if (should_send_keyboard_report) {
          should_send_keyboard_report = 0;
          tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
        }
      }
    }

#if IS_DRV2605_ENABLED
    if (key_triggered) {
      writeRegister8(DRV2605_REG_WAVESEQ1 + 0, 1);
      writeRegister8(DRV2605_REG_WAVESEQ1 + 1, 0);
      writeRegister8(DRV2605_REG_GO, 1);
    }
#endif
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
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
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
  /* USER CODE BEGIN ADC1_Init 2 */

  // memcpy(&ADC_channel_Config, &sConfig, sizeof(ADC_ChannelConfTypeDef));

  /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
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
uint8_t get_bitmask_for_modifier(uint8_t keycode) {
  switch (keycode) {
  case HID_KEY_CONTROL_LEFT:
    return 0b00000001;
  case HID_KEY_SHIFT_LEFT:
    return 0b00000010;
  case HID_KEY_ALT_LEFT:
    return 0b00000100;
  case HID_KEY_GUI_LEFT:
    return 0b00001000;
  case HID_KEY_CONTROL_RIGHT:
    return 0b00010000;
  case HID_KEY_SHIFT_RIGHT:
    return 0b00100000;
  case HID_KEY_ALT_RIGHT:
    return 0b01000000;
  case HID_KEY_GUI_RIGHT:
    return 0b10000000;
  default:
    return 0b00000000;
  }
}

uint16_t get_usage_consumer_control(uint16_t value) {
  if (value > 0xFF) {
    return value & 0b0111111111111111;
  } else {
    return 0;
  }
}

void init_key(uint8_t adc_channel, uint8_t amux_channel, uint8_t row, uint8_t column) {
  struct key *key = &keys[adc_channel][amux_channel];

  key->is_enabled = 1;
  key->is_idle = 0;
  key->row = row;
  key->column = column;

  key->calibration.cycles_count = 0;
  key->calibration.idle_value = IDLE_VALUE_APPROX;
  key->calibration.max_distance = MAX_DISTANCE_APPROX;

  key->actuation.status = STATUS_RESET;
  key->actuation.trigger_offset = user_config.trigger_offset;
  key->actuation.reset_offset = user_config.trigger_offset - user_config.reset_threshold;
  key->actuation.rapid_trigger_offset = user_config.rapid_trigger_offset;
  key->actuation.is_continuous_rapid_trigger_enabled = 0;

  for (uint8_t i = 0; i < LAYERS_COUNT; i++) {
    if (user_config.keymaps[i][row][column] != ____) {
      uint16_t usage_consumer_control = get_usage_consumer_control(user_config.keymaps[i][row][column]);
      if (usage_consumer_control) {
        key->layers[i].type = KEY_TYPE_CONSUMER_CONTROL;
        key->layers[i].value = usage_consumer_control;
      } else {
        uint8_t bitmask = get_bitmask_for_modifier(user_config.keymaps[i][row][column]);
        if (bitmask) {
          key->layers[i].type = KEY_TYPE_MODIFIER;
          key->layers[i].value = bitmask;
        } else {
          key->layers[i].type = KEY_TYPE_NORMAL;
          key->layers[i].value = user_config.keymaps[i][row][column];
        }
      }
    }
  }
}

void init_keys() {
  readConfig();

  ADC_channel_Config.Rank = 1;
  ADC_channel_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (channels_by_row_col[row][col][0] != XXXX) {
        init_key(channels_by_row_col[row][col][0], channels_by_row_col[row][col][1], row, col);
      }
    }
  }
}

void add_to_hid_report(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers |= key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == 0) {
        keycodes[i] = key->layers[layer].value;
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = key->layers[layer].value;
    should_send_consumer_report = 1;
    break;
  }
}

void remove_from_hid_report(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers &= ~key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == key->layers[layer].value) {
        keycodes[i] = 0;
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = 0;
    should_send_consumer_report = 1;
    break;
  }
}

uint8_t update_key_state(struct key *key) {
  struct state state;

  // Get a reading
  state.value = HAL_ADC_GetValue(&hadc1);

  if (key->calibration.cycles_count < CALIBRATION_CYCLES) {
    // Calibrate idle value
    float delta = 0.6;
    key->calibration.idle_value = (1 - delta) * state.value + delta * key->calibration.idle_value;
    key->calibration.cycles_count++;

    return 0;
  }

  // Calibrate idle value
  if (state.value > key->calibration.idle_value) {
    // opti possible sur float
    float delta = 0.8;
    key->calibration.idle_value = (1 - delta) * state.value + delta * key->calibration.idle_value;
    state.value = key->calibration.idle_value;
  }

  // Do nothing if key is idle
  if (key->state.distance == 0 && state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
    if (key->idle_counter >= IDLE_CYCLES_UNTIL_SLEEP) {
      key->is_idle = 1;
      return 0;
    }
    key->idle_counter++;
  }

  // Get distance from top
  if (state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
    state.distance = 0;
    key->actuation.direction_changed_point = 0;
  } else {
    state.distance = key->calibration.idle_value - IDLE_VALUE_OFFSET - state.value;
    key->is_idle = 0;
    key->idle_counter = 0;
  }

  // Calibrate max distance value
  if (state.distance > key->calibration.max_distance) {
    key->calibration.max_distance = state.distance;
  }

  // Limit max distance
  if (state.distance >= key->calibration.max_distance - MAX_DISTANCE_OFFSET) {
    state.distance = key->calibration.max_distance;
  }

  // Map distance in percentages
  state.distance_8bits = (state.distance * 0xff) / key->calibration.max_distance;

  float delta = 0.8;
  state.filtered_distance = (1 - delta) * state.distance_8bits + delta * key->state.filtered_distance;
  state.filtered_distance_8bits = state.filtered_distance;

  // Update velocity
  state.velocity = state.filtered_distance_8bits - key->state.filtered_distance_8bits;

  // Update direction
  if (key->state.velocity > 0 && state.velocity > 0 && key->actuation.direction != GOING_DOWN) {
    key->actuation.direction = GOING_DOWN;
    if (key->actuation.direction_changed_point != 0) {
      key->actuation.direction_changed_point = key->state.filtered_distance_8bits;
    }
  } else if (key->state.velocity < 0 && state.velocity < 0 && key->actuation.direction != GOING_UP) {
    key->actuation.direction = GOING_UP;
    if (key->actuation.direction_changed_point != 255) {
      key->actuation.direction_changed_point = key->state.filtered_distance_8bits;
    }
  }

  key->state = state;
  return 1;
}

void update_key_actuation(struct key *key) {
  /**
   * https://www.youtube.com/watch?v=_Sl-T6iQr8U&t
   *
   *                          -----   |--------|                           -
   *                            |     |        |                           |
   *    is_before_reset_offset  |     |        |                           |
   *                            |     |        |                           | Continuous rapid trigger domain (deactivated when full_reset)
   *                          -----   | ------ | <- reset_offset           |
   *                            |     |        |                           |
   *                          -----   | ------ | <- trigger_offset         -
   *                            |     |        |                           |
   *                            |     |        |                           |
   *   is_after_trigger_offset  |     |        |                           | Rapid trigger domain
   *                            |     |        |                           |
   *                            |     |        |                           |
   *                          -----   |--------|                           -
   *
   */

  // if rapid trigger enable, move trigger and reset offsets according to the distance taht began the trigger

  uint32_t now = HAL_GetTick();
  uint8_t is_after_trigger_offset = key->state.distance_8bits > key->actuation.trigger_offset;
  uint8_t is_before_reset_offset = key->state.distance_8bits < key->actuation.reset_offset;
  uint8_t has_rapid_trigger = key->actuation.rapid_trigger_offset != 0;
  uint8_t is_after_rapid_trigger_offset = key->state.distance_8bits > key->actuation.direction_changed_point - key->actuation.rapid_trigger_offset + user_config.reset_threshold;
  uint8_t is_before_rapid_reset_offset = key->state.distance_8bits < key->actuation.direction_changed_point - key->actuation.rapid_trigger_offset;

  switch (key->actuation.status) {

  case STATUS_RESET:
    // if reset, can be triggered or tap
    if (is_after_trigger_offset) {
      if (key->layers[_TAP_LAYER].value) {
        key->actuation.status = STATUS_MIGHT_BE_TAP;
        // key_triggered = 1;
      } else {
        key->actuation.status = STATUS_TRIGGERED;
        key_triggered = 1;
        add_to_hid_report(key, _BASE_LAYER);
      }
      key->actuation.triggered_at = now;
    }
    break;

  case STATUS_RAPID_TRIGGER_RESET:
    if (!has_rapid_trigger) {
      key->actuation.status = STATUS_RESET;
      break;
    }
    // if reset, can be triggered or tap
    if (is_after_trigger_offset && key->actuation.direction == GOING_DOWN && is_after_rapid_trigger_offset) {
      if (key->layers[_TAP_LAYER].value) {
        key->actuation.status = STATUS_MIGHT_BE_TAP;
        key_triggered = 1;
      } else {
        key->actuation.status = STATUS_TRIGGERED;
        key_triggered = 1;
        add_to_hid_report(key, _BASE_LAYER);
      }
      key->actuation.triggered_at = now;
    } else if (is_before_reset_offset) {
      key->actuation.status = STATUS_RESET;
    }
    break;

  case STATUS_TAP:
    // if tap, can be reset
    key->actuation.status = STATUS_RESET;
    remove_from_hid_report(key, _TAP_LAYER);
    break;

  case STATUS_TRIGGERED:
    // if triggered, can be reset
    if (is_before_reset_offset) {
      key->actuation.status = STATUS_RESET;
      remove_from_hid_report(key, _BASE_LAYER);
    } else if (has_rapid_trigger && key->actuation.direction == GOING_UP && is_before_rapid_reset_offset) {
      key->actuation.status = STATUS_RAPID_TRIGGER_RESET;
      remove_from_hid_report(key, _BASE_LAYER);
    }
    break;
  }
}

void update_key(struct key *key) {
  if (!update_key_state(key)) {
    return;
  }

  update_key_actuation(key);
}

// MARK: tud_* functions

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
  // if (instance == 1 && report_id == 0) {
  //   writeConfig(buffer, bufsize);

  //   init_keys();
  // }
}

// // Invoked when cdc when line state changed e.g connected/disconnected
// void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
//   (void)itf;

//   // connected
//   if (dtr && rts) {
//     // print initial message when connected
//     tud_cdc_write(&user_config, 3);
//     tud_cdc_write_str('\r\n');
//     tud_cdc_write_flush();
//   }
// }

// // Invoked when CDC interface received data from host
// void tud_cdc_rx_cb(uint8_t itf) {
//   (void)itf;
// }

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
  switch (request->bmRequestType_bit.type) {
  case TUSB_REQ_TYPE_VENDOR: {
    switch (request->bRequest) {

    case VENDOR_REQUEST_CONFIG: {
      switch (request->wValue) {
      case VENDOR_VALUE_GET_LENGTH: {
        if (stage == CONTROL_STAGE_SETUP) {
          uint16_t size = sizeof(user_config);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &user_config, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_SET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, usb_vendor_control_buffer, request->wLength);
        } else if (stage == CONTROL_STAGE_DATA) {
          if (!writeConfig(&usb_vendor_control_buffer, 0, request->wLength)) {
            return false;
          }
          readConfig();
          init_keys();
        }

        break;
      }

      default:
        break;
      }
    }

    case VENDOR_REQUEST_RESET_CONFIG: {
      if (request->wValue == VENDOR_VALUE_SET) {
        if (stage == CONTROL_STAGE_SETUP) {
          if (!writeConfig(&default_user_config, 0, sizeof default_user_config)) {
            return false;
          }
          readConfig();
          init_keys();
          return tud_control_status(rhport, request);
        }

        break;
      }
    }

    case VENDOR_REQUEST_KEYS: {
      switch (request->wValue) {
      case VENDOR_VALUE_GET_LENGTH: {
        if (stage == CONTROL_STAGE_SETUP) {
          uint16_t size = sizeof(keys);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &keys, request->wLength);
        }

        break;
      }

      default:
        break;
      }
    }

    case VENDOR_REQUEST_WEBUSB: {
      if (stage == CONTROL_STAGE_SETUP) {
        return tud_control_status(rhport, request);
      }

      break;
    }

    case VENDOR_REQUEST_MICROSOFT: {
      if (stage == CONTROL_STAGE_SETUP) {
        if (request->wIndex == 7) {
          // Get Microsoft OS 2.0 compatible descriptor
          uint16_t total_len;
          memcpy(&total_len, desc_ms_os_20 + 8, 2);

          return tud_control_xfer(rhport, request, (void *)(uintptr_t)desc_ms_os_20, total_len);
        }

        return false;
      }

      break;
    }

    default:
      break;
    }
  }

  case TUSB_REQ_TYPE_CLASS: {
    if (stage == CONTROL_STAGE_SETUP) {
      if (request->bRequest == 0x22) {
        // response with status OK
        return tud_control_status(rhport, request);
      }

      return false;
    }

    break;
  }

  default:
    break;
  }

  if (stage != CONTROL_STAGE_SETUP) {
    return true;
  }
  return false;
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
