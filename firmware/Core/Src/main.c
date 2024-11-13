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
#include "keyboard.h"
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
#define VENDOR_REQUEST_DFU_MODE 0xfc

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

const uint32_t adc_channels[ADC_CHANNEL_COUNT] = {ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7};

const uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// https://stm32f4-discovery.net/2017/04/tutorial-jump-system-memory-software-stm32/
void jump_to_bootloader(void) {
  __enable_irq();
  HAL_RCC_DeInit();
  HAL_DeInit();
  SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

  const uint32_t p = (*((uint32_t *)0x1FFF0000));
  __set_MSP(p);

  void (*SysMemBootJump)(void);
  SysMemBootJump = (void (*)(void))(*((uint32_t *)0x1FFF0004));
  SysMemBootJump();

  while (1) {
  }
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

  ADC_channel_Config.Rank = 1;
  ADC_channel_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  keyboard_init_keys();

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

    keyboard_task();

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

// MARK: keyboard_* functions

void keyboard_read_config() {
  memcpy(&keyboard_user_config, (uint32_t *)CONFIG_ADDRESS, sizeof(keyboard_user_config));
}

uint8_t keyboard_write_config(uint8_t *buffer, uint16_t offset, uint16_t size) {
  if (offset >= sizeof keyboard_user_config) {
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

void keyboard_select_amux(uint8_t amux_channel) {
  // TODO: set GPIOs at the same time using bitmap on register
  for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
    HAL_GPIO_WritePin(GPIOB, amux_select_pins[i], (amux_channel >> i) & 1);
  }
}

void keyboard_select_adc(uint8_t adc_channel) {
  ADC_channel_Config.Channel = adc_channels[adc_channel];
  HAL_ADC_ConfigChannel(&hadc1, &ADC_channel_Config);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 1);
}

uint16_t keyboard_read_adc() {
  return HAL_ADC_GetValue(&hadc1);
}

void keyboard_close_adc() {
  HAL_ADC_Stop(&hadc1);
}

uint32_t keyboard_get_time() {
  return HAL_GetTick();
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
//     tud_cdc_write(&keyboard_user_config, 3);
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
          uint16_t size = sizeof(keyboard_user_config);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &keyboard_user_config, request->wLength);
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
          if (!writeConfig(&default_keyboard_user_config, 0, sizeof default_keyboard_user_config)) {
            return false;
          }
          readConfig();
          init_keys();
          return tud_control_status(rhport, request);
        }

        break;
      }
    }

    case VENDOR_REQUEST_DFU_MODE: {
      if (request->wValue == VENDOR_VALUE_SET) {
        if (stage == CONTROL_STAGE_SETUP) {
          jump_to_bootloader();
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
