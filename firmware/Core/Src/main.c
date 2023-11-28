#include "main.h"
#include "keys.c"
#include "keys.h"
#include "layout.c"
#include "tusb.h"
#include "usb_descriptors.h"

// ADC_HandleTypeDef hadc1;
// I2C_HandleTypeDef hi2c1;
PCD_HandleTypeDef hpcd_USB_OTG_FS;

static uint8_t should_send_report = 0;
static uint8_t can_send_report = 0;
static uint32_t last_report_sent_at = 0;

static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {0, 0, 0, 0, 0, 0};
static uint8_t current_keycode_index = 0;
static uint8_t report[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
// static void MX_ADC1_Init(void);
// static void MX_I2C1_Init(void);

int main(void) {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  // MX_ADC1_Init();
  // MX_I2C1_Init();
  // MX_USB_DEVICE_Init();
  MX_USB_OTG_FS_PCD_Init();

  // uint32_t USB_delay = USBD_HID_GetPollingInterval(&hpcd_USB_OTG_FS);

  HAL_Delay(1000);

  keys_init(25, 0, 0);

  HAL_Delay(1000);
  can_send_report = 1;
  // CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  // DWT->CYCCNT = 0;
  // DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  while (1) {
    // unsigned long t1 = DWT->CYCCNT;
    tud_task();

    keys_loop();

    if (tud_hid_ready() && should_send_report && can_send_report) {
      if (tud_suspended()) {
        tud_remote_wakeup();
      } else {
        can_send_report = 0;
        should_send_report = 0;
        tud_hid_keyboard_report(ITF_NUM_KEYBOARD, modifiers, keycodes);
        report[0] = modifiers;
        for (uint8_t i = 0; i < 6; i++) {
          report[2 + i] = keycodes[i];
        }
      }
    }

    // uint32_t now = HAL_GetTick();
    // // if (should_send_report && now - last_report_sent_at > USB_delay) {
    // if (should_send_report) {
    //   report[0] = modifiers;
    //   for (uint8_t i = 0; i < 6; i++) {
    //     report[2 + i] = keycodes[i];
    //   }
    //   // USBD_HID_SendReport(&hUsbDeviceFS, &report, sizeof(report));

    //   last_report_sent_at = now;
    //   should_send_report = 0;
    //   // HAL_Delay(USB_delay);
    // }
    // HAL_Delay(10);

    // unsigned long t2 = DWT->CYCCNT;
    // unsigned long cycles = t2 - t1;
    // duration = cycles / 60;
  }
}

void keys_on_change(struct key *key) {
  if (keymaps[_BASE_LAYER][key->row][key->column] == XXXX) {
    return;
  }

  if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_CONTROL_LEFT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00000001;
    } else {
      modifiers &= 0b11111110;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_SHIFT_LEFT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00000010;
    } else {
      modifiers &= 0b11111101;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_ALT_LEFT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00000100;
    } else {
      modifiers &= 0b11111011;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_GUI_LEFT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00001000;
    } else {
      modifiers &= 0b11110111;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_CONTROL_RIGHT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00010000;
    } else {
      modifiers &= 0b11101111;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_SHIFT_RIGHT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b00100000;
    } else {
      modifiers &= 0b11011111;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_ALT_RIGHT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b01000000;
    } else {
      modifiers &= 0b10111111;
    }
  } else if (keymaps[_BASE_LAYER][key->row][key->column] == HID_KEY_GUI_RIGHT) {
    if (!key->actuation.is_reset) {
      modifiers |= 0b10000000;
    } else {
      modifiers &= 0b01111111;
    }
  } else {
    if (key->actuation.is_reset == 0) {
      if (current_keycode_index < 6) {
        keycodes[current_keycode_index] = keymaps[_BASE_LAYER][key->row][key->column];
        current_keycode_index++;
      }
    } else {
      uint8_t new_keycodes[6] = {0, 0, 0, 0, 0, 0};
      uint8_t new_keycode_index = 0;
      for (uint8_t i = 0; i < 6; i++) {
        if (keycodes[i] != keymaps[_BASE_LAYER][key->row][key->column] && keycodes[i] != 0) {
          new_keycodes[new_keycode_index] = keycodes[i];
          new_keycode_index++;
        }
      }

      for (uint8_t i = 0; i < 6; i++) {
        keycodes[i] = new_keycodes[i];
      }

      current_keycode_index = new_keycode_index;
    }
  }

  should_send_report = 1;
}

void keys_on_triggered(struct key *key) {
}

void keys_on_reset(struct key *key) {
}

static void SystemClock_Config(void) {
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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 370;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
}

// static void MX_ADC1_Init(void) {
//   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
//    */
//   hadc1.Instance = ADC1;
//   hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
//   hadc1.Init.Resolution = ADC_RESOLUTION_12B;
//   hadc1.Init.ScanConvMode = DISABLE;
//   hadc1.Init.ContinuousConvMode = DISABLE;
//   hadc1.Init.DiscontinuousConvMode = ENABLE;
//   hadc1.Init.DiscontinuousConvMode = DISABLE;
//   hadc1.Init.NbrOfDiscConversion = 1;
//   hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//   hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//   hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//   hadc1.Init.NbrOfConversion = 1;
//   hadc1.Init.DMAContinuousRequests = DISABLE;
//   hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//   if (HAL_ADC_Init(&hadc1) != HAL_OK) {
//     Error_Handler();
//   }
// }

// static void MX_I2C1_Init(void) {
//   hi2c1.Instance = I2C1;
//   hi2c1.Init.ClockSpeed = 100000;
//   hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
//   hi2c1.Init.OwnAddress1 = 0;
//   hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//   hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//   hi2c1.Init.OwnAddress2 = 0;
//   hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//   hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//   if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
//     Error_Handler();
//   }
// }

static void MX_USB_OTG_FS_PCD_Init(void) {
  // SysTick_Config(SystemCoreClock / 1000);

  // GPIO_InitTypeDef GPIO_InitStruct;

  // /* Configure USB D+ D- Pins */
  // GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  // GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  // GPIO_InitStruct.Pull = GPIO_NOPULL;
  // GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // // /* Configure VBUS Pin */
  // // GPIO_InitStruct.Pin = GPIO_PIN_9;
  // // GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  // // GPIO_InitStruct.Pull = GPIO_NOPULL;
  // // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // // /* ID Pin */
  // // GPIO_InitStruct.Pin = GPIO_PIN_10;
  // // GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  // // GPIO_InitStruct.Pull = GPIO_PULLUP;
  // // GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  // // GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  // // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // // Enable USB OTG clock
  // __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

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

  tud_init(BOARD_TUD_RHPORT);
}

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

void Error_Handler(void) {
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
