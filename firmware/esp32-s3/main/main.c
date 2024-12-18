#include "config.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hid.h"
#include "keyboard.h"
#include "soc/soc_caps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const static char *TAG = "MACROLEV";

#define CONFIG_ADDRESS 0x42000000

#define GPIO_SELECT_0 5
#define GPIO_SELECT_1 6
#define GPIO_SELECT_2 9
#define GPIO_SELECT_3 10
/*
 * Let's say, GPIO_SELECT_0=18, GPIO_SELECT_1=19
 * In binary representation,
 * 1ULL<<GPIO_SELECT_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_SELECT_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL                0000000000000000000011000000000000000000
 * */
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_SELECT_0) | (1ULL << GPIO_SELECT_1) | (1ULL << GPIO_SELECT_2) | (1ULL << GPIO_SELECT_3))

adc_oneshot_unit_handle_t adc_handle;
adc_channel_t adc_channel;

extern struct user_config keyboard_user_config;

const uint32_t adc_channels[ADC_CHANNEL_COUNT] = {ADC_CHANNEL_7, ADC_CHANNEL_6, ADC_CHANNEL_5, ADC_CHANNEL_4, ADC_CHANNEL_3};
const uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = {GPIO_SELECT_0, GPIO_SELECT_1, GPIO_SELECT_2, GPIO_SELECT_3};

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle) {
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
  if (!calibrated) {
    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
  if (!calibrated) {
    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif

  *out_handle = handle;
  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Calibration Success");
  } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
  } else {
    ESP_LOGE(TAG, "Invalid arg or no memory");
  }

  return calibrated;
}

static void adc_init() {
  //-------------ADC Init---------------//
  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = ADC_UNIT_2,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

  //-------------ADC Config---------------//
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_12,
  };

  for (uint8_t i = 0; i < ADC_CHANNEL_COUNT; i++) {
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, adc_channels[i], &config));
    //-------------ADC Calibration Init---------------//
    adc_cali_handle_t adc_cali_chan_handle = NULL;
    adc_calibration_init(ADC_UNIT_2, adc_channels[i], ADC_ATTEN_DB_12, &adc_cali_chan_handle);
  }
}

void gpio_init() {
  // zero-initialize the config structure.
  gpio_config_t gpio_conf = {
      // disable interrupt
      .intr_type = GPIO_INTR_DISABLE,
      // set as output mode
      .mode = GPIO_MODE_OUTPUT,
      // bit mask of the pins that you want to set
      .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
      // disable pull-down mode
      .pull_down_en = 0,
      // disable pull-up mode
      .pull_up_en = 0,
  };

  gpio_config(&gpio_conf);
}

void app_main(void) {
  gpio_init();
  adc_init();
  keyboard_init_keys();
  hid_init();
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // gpio_reset_pin(GPIO_SELECT_0);
  // gpio_set_direction(GPIO_SELECT_0, GPIO_MODE_OUTPUT);
  // while (1) {
  //   gpio_set_level(GPIO_SELECT_0, 1);
  //   vTaskDelay(pdMS_TO_TICKS(100));
  // }

  while (1) {
    keyboard_task();
    hid_task();

    // for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    //   for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
    //     gpio_set_level(amux_select_pins[i], (amux_channel >> i) & 1);
    //   }

    //   for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
    //     int raw_value = 0;
    //     adc_oneshot_read(adc_handle, adc_channels[adc_channel], &raw_value);
    //   }
    // }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void keyboard_select_adc(uint8_t _adc_channel) {
  adc_channel = _adc_channel;
}

void keyboard_select_amux(uint8_t amux_channel) {
  for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
    gpio_set_level(amux_select_pins[i], (amux_channel >> i) & 1);
  }
}

uint16_t keyboard_read_adc() {
  int raw_value = 0;
  adc_oneshot_read(adc_handle, adc_channels[adc_channel], &raw_value);
  return raw_value;
}

void keyboard_close_adc() {
}

uint32_t keyboard_get_time() {
  return esp_timer_get_time();
}

void keyboard_read_config() {
  memcpy(&keyboard_user_config, &keyboard_default_user_config, sizeof(keyboard_user_config));
}

uint8_t keyboard_write_config(uint8_t *buffer, uint16_t offset, uint16_t size) {
  return 1;
}
