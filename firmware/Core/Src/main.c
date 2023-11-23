#include "main.h"
#include "layout.c"
#include "usb_device.h"
#include "usbd_hid.h"

ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

extern USBD_HandleTypeDef hUsbDeviceFS;

// typedef struct
// {
//   uint8_t MODIFIER;
//   uint8_t RESERVED;
//   uint8_t KEYCODE1;
//   uint8_t KEYCODE2;
//   uint8_t KEYCODE3;
//   uint8_t KEYCODE4;
//   uint8_t KEYCODE5;
//   uint8_t KEYCODE6;
// } subKeyBoard;

// subKeyBoard keyBoardHIDsub = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t report[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t current_keycode_index = 0;
static uint8_t can_send_empty_report = 0;

uint16_t min_raw = 3000;
uint16_t max_raw = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);

/* Private user code ---------------------------------------------------------*/

static uint8_t HIDInEpAdd = HID_EPIN_ADDR;
/**
 * @brief  USBD_HID_SendReport
 *         Send HID Report
 * @param  pdev: device instance
 * @param  buff: pointer to report
 * @retval status
 */
uint8_t USBD_HID_SendReport_Custom(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len) {
  USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hhid == NULL) {
    return (uint8_t)USBD_FAIL;
  }

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  HIDInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_state == USBD_STATE_CONFIGURED) {
    if (hhid->state == HID_IDLE) {
      hhid->state = HID_BUSY;
      (void)USBD_LL_Transmit(pdev, HIDInEpAdd, report, len);
    }
    return USBD_BUSY;
  }

  return (uint8_t)USBD_OK;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();

  uint16_t trigger = 1600;

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  while (1) {
    unsigned long t1 = DWT->CYCCNT;

    for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
      uint8_t a = (amux_channel >> 0) & 1;
      uint8_t b = (amux_channel >> 1) & 1;
      uint8_t c = (amux_channel >> 2) & 1;
      uint8_t d = (amux_channel >> 3) & 1;
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, a);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, b);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, c);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, d);

      for (uint8_t channel = 0; channel < ADC_CHANNEL_COUNT; channel++) {

        if (row_col_by_channels[channel][amux_channel][0] == 99) {
          // no hall effect sensor on this channel
          continue;
        }

        ADC_ChannelConfTypeDef sConfig = {0};
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        if (channel == 0)
          sConfig.Channel = ADC_CHANNEL_3;
        else if (channel == 1)
          sConfig.Channel = ADC_CHANNEL_4;
        else if (channel == 2)
          sConfig.Channel = ADC_CHANNEL_5;
        else if (channel == 3)
          sConfig.Channel = ADC_CHANNEL_6;
        else if (channel == 4)
          sConfig.Channel = ADC_CHANNEL_7;
        sConfig.Rank = 1;
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
          Error_Handler();
        }
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        uint16_t raw = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);

        if (channel == 0 && amux_channel == 10) {
          if (raw < min_raw) {
            min_raw = raw;
          }
          if (raw > max_raw) {
            max_raw = raw;
          }
        }

        if (raw < trigger && current_keycode_index <= 6) {
          uint8_t row = row_col_by_channels[channel][amux_channel][0];
          uint8_t col = row_col_by_channels[channel][amux_channel][1];
          uint8_t keycode = keymaps[_BASE_LAYER][row][col];
          if (keycode != 0) {
            report[2 + current_keycode_index] = keycode;
            current_keycode_index++;
          }
        }
      }
    }

    if (current_keycode_index > 0) {
      USBD_HID_SendReport_Custom(&hUsbDeviceFS, &report, sizeof(report));
      current_keycode_index = 0;
      for (uint8_t i = 0; i <= 8; i++) {
        report[i] = 0;
      }
      can_send_empty_report = 1;
    } else if (can_send_empty_report) {
      if (USBD_HID_SendReport_Custom(&hUsbDeviceFS, &report, sizeof(report)) == USBD_OK) {
        can_send_empty_report = 0;
      }
    }

    // send empty report to release keys if no keys are pressed
    // send empty report to release keys if no keys are pressed
    // send empty report to release keys if no keys are pressed
    // send empty report to release keys if no keys are pressed
    // send empty report to release keys if no keys are pressed

    unsigned long t2 = DWT->CYCCNT;
    unsigned long cycles = t2 - t1;
    unsigned long duration = cycles / 60;
    HAL_Delay(1);
  }
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }

  /** Enables the Clock Security System
   */
  HAL_RCC_EnableCSS();
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {
  // ADC_ChannelConfTypeDef sConfig = {0};
  // sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  // hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  // hadc1.Init.NbrOfConversion = 5;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }

  // /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  //  */
  // sConfig.Channel = ADC_CHANNEL_3;
  // sConfig.Rank = 1;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
  //   Error_Handler();
  // }

  // /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  //  */
  // sConfig.Channel = ADC_CHANNEL_4;
  // sConfig.Rank = 2;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
  //   Error_Handler();
  // }

  // /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  //  */
  // sConfig.Channel = ADC_CHANNEL_5;
  // sConfig.Rank = 3;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
  //   Error_Handler();
  // }

  // /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  //  */
  // sConfig.Channel = ADC_CHANNEL_6;
  // sConfig.Rank = 4;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
  //   Error_Handler();
  // }

  // /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  //  */
  // sConfig.Channel = ADC_CHANNEL_7;
  // sConfig.Rank = 5;
  // if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
  //   Error_Handler();
  // }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {
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
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

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
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
