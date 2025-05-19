#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>

/**
 * chip dependent
 * - usb
 * - config r/w
 * - how to read adc values
 *
 * lib
 * - config parse
 * - runtime
 */

static const char *TAG = "MACROLEV";

/**
 * @todo read the config and compute the max amux channel
 * @todo read the config and compute the max amux channel
 * @todo read the config and compute the max amux channel
 * @todo read the config and compute the max amux channel
 */
#define AMUX_CHANNEL_COUNT 14

/**
 * @todo read the config and compute the adc channel list
 * @todo read the config and compute the adc channel list
 * @todo read the config and compute the adc channel list
 * @todo read the config and compute the adc channel list
 */
enum adc_channel {
  MLEV_ADC_CHANNEL_0 = ADC_CHANNEL_2,
  MLEV_ADC_CHANNEL_1 = ADC_CHANNEL_3,
  MLEV_ADC_CHANNEL_2 = ADC_CHANNEL_4,
  MLEV_ADC_CHANNEL_3 = ADC_CHANNEL_5,
  MLEV_ADC_CHANNEL_4 = ADC_CHANNEL_8,
  MLEV_ADC_CHANNEL_COUNT,
};
const uint32_t adc_channels[MLEV_ADC_CHANNEL_COUNT] = { MLEV_ADC_CHANNEL_0, MLEV_ADC_CHANNEL_1, MLEV_ADC_CHANNEL_2, MLEV_ADC_CHANNEL_3, MLEV_ADC_CHANNEL_4 };

/**
 * @todo read the config and compute the amux select pin list
 * @todo read the config and compute the amux select pin list
 * @todo read the config and compute the amux select pin list
 * @todo read the config and compute the amux select pin list
 */
enum amux_select_pin {
  AMUX_SELECT_PIN_0 = GPIO_NUM_15,
  AMUX_SELECT_PIN_1 = GPIO_NUM_16,
  AMUX_SELECT_PIN_2 = GPIO_NUM_17,
  AMUX_SELECT_PIN_3 = GPIO_NUM_18,
  AMUX_SELECT_PIN_COUNT,
};
const uint32_t amux_select_pins[AMUX_SELECT_PIN_COUNT] = { AMUX_SELECT_PIN_0, AMUX_SELECT_PIN_1, AMUX_SELECT_PIN_2, AMUX_SELECT_PIN_3 };

/*
 * Let's say, AMUX_SELECT_0=18, AMUX_SELECT_1=19
 * In binary representation,
 * 1ULL<<AMUX_SELECT_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<AMUX_SELECT_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL             0000000000000000000011000000000000000000
 * */
#define GPIO_OUTPUT_PIN_SEL ((1ULL << AMUX_SELECT_PIN_0) | (1ULL << AMUX_SELECT_PIN_1) | (1ULL << AMUX_SELECT_PIN_2) | (1ULL << AMUX_SELECT_PIN_3))

#define CONVERSION_FRAME_SIZE (SOC_ADC_DIGI_DATA_BYTES_PER_CONV * MLEV_ADC_CHANNEL_COUNT)
#define CONVERSION_POOL_SIZE CONVERSION_FRAME_SIZE * 1

adc_continuous_handle_t adc_handle;
uint8_t current_amux_channel = 0;
static TaskHandle_t adc_task_handle;

static bool IRAM_ATTR on_conversion_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
  BaseType_t mustYield = pdFALSE;
  // Notify that ADC continuous driver has done enough number of conversions
  vTaskNotifyGiveFromISR(adc_task_handle, &mustYield);

  return (mustYield == pdTRUE);
}

static void adc_init() {
  //-------------ADC Init---------------//
  adc_continuous_handle_cfg_t adc_config = {
    .max_store_buf_size = CONVERSION_POOL_SIZE,
    .conv_frame_size = CONVERSION_FRAME_SIZE,
    .flags = {
        .flush_pool = 1 }
  };
  ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

  //-------------ADC Config---------------//
  adc_continuous_config_t config = {
    .pattern_num = MLEV_ADC_CHANNEL_COUNT,
    .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_HIGH,
    .conv_mode = ADC_CONV_SINGLE_UNIT_1,
    .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
  };

  adc_digi_pattern_config_t adc_pattern[MLEV_ADC_CHANNEL_COUNT] = { 0 };
  for (int i = 0; i < MLEV_ADC_CHANNEL_COUNT; i++) {
    adc_pattern[i].atten = ADC_ATTEN_DB_12;
    adc_pattern[i].channel = adc_channels[i];
    adc_pattern[i].unit = ADC_UNIT_1;
    adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

    ESP_LOGI(TAG, "adc_pattern[%d].channel is :%" PRIx8, i, adc_pattern[i].channel);
  }
  config.adc_pattern = adc_pattern;
  ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &config));

  adc_continuous_evt_cbs_t callbacks = {
    .on_conv_done = on_conversion_done_cb,
  };
  ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &callbacks, NULL));
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

  for (uint8_t i = 0; i < AMUX_SELECT_PIN_COUNT; i++) {
    gpio_set_level(amux_select_pins[i], 0);
  }
}

void adc_task(void *pvParameters) {
  adc_task_handle = xTaskGetCurrentTaskHandle();
  uint32_t conversion_frame_real_size = 0;
  uint8_t conversions[CONVERSION_FRAME_SIZE] = { 0 };
  memset(conversions, 0, CONVERSION_FRAME_SIZE);

  while (1) {
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
    vTaskDelay(1);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    adc_continuous_read(adc_handle, conversions, CONVERSION_FRAME_SIZE, &conversion_frame_real_size, 0);

    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));

    uint8_t amux_channel = current_amux_channel;

    current_amux_channel = (amux_channel + 1) % AMUX_CHANNEL_COUNT;
    for (uint8_t i = 0; i < AMUX_SELECT_PIN_COUNT; i++) {
      gpio_set_level(amux_select_pins[i], (current_amux_channel >> i) & 1);
    }

    for (uint8_t conversion_result_index = 0; conversion_result_index < conversion_frame_real_size; conversion_result_index += SOC_ADC_DIGI_DATA_BYTES_PER_CONV) {
      adc_digi_output_data_t *conversion_frame = (adc_digi_output_data_t *)&conversions[conversion_result_index];
      for (uint8_t adc_channel = 0; adc_channel < MLEV_ADC_CHANNEL_COUNT; adc_channel++) {
        if (conversion_frame->type2.channel == adc_channels[adc_channel]) {
          mlev_set_switch_value(adc_channel, amux_channel, conversion_frame->type2.data);
          break;
        }
      }
    }
  }
}

// void debug_task(void *pvParameters) {
//   while (1) {
//     printf("\033[H\033[J");
//     printf("(%d)\n", current_amux_channel);
//     for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
//       for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
//         printf("%2d:%2d %4d | ", adc_channel, amux_channel, adc_buffer[adc_channel][amux_channel]);
//       }
//       printf("\n");
//     }
//     vTaskDelay(pdMS_TO_TICKS(100));
//   }
// }