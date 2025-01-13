#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "macrolev.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include <stdio.h>
#include <string.h>

const static char *TAG = "MACROLEV";

// {adc_channel, amux_channel}
// const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
//     {{0, 1}, {0, 0}, {0, 13}, {0, 12}, {1, 0}, {1, 12}, {2, 1}, {2, 0}, {2, 12}, {3, 1}, {3, 0}, {3, 12}, {4, 1}, {4, 0}, {4, 13}},
//     {{0, 2}, {0, 7}, {0, 10}, {1, 2}, {1, 7}, {1, 10}, {2, 3}, {2, 7}, {2, 10}, {3, 3}, {3, 7}, {3, 11}, {4, 4}, {4, 8}, {4, 12}},
//     {{0, 3}, {0, 6}, {0, 11}, {1, 3}, {1, 6}, {1, 11}, {2, 4}, {2, 6}, {2, 11}, {3, 4}, {3, 8}, {4, 2}, {4, 5}, {4, 7}, {XXXX, XXXX}},
//     {{XXXX, XXXX}, {0, 5}, {0, 8}, {1, 1}, {1, 5}, {1, 8}, {2, 2}, {2, 5}, {2, 8}, {3, 2}, {3, 6}, {3, 9}, {4, 3}, {4, 10}, {XXXX, XXXX}},
//     {{XXXX, XXXX}, {0, 4}, {0, 9}, {1, 4}, {XXXX, XXXX}, {1, 9}, {XXXX, XXXX}, {2, 9}, {XXXX, XXXX}, {3, 5}, {3, 10}, {XXXX, XXXX}, {4, 6}, {4, 9}, {4, 11}},
// };

#define AMUX_SELECT_PINS_COUNT 4
#define AMUX_CHANNEL_COUNT 14
#define ADC_CHANNEL_COUNT 5

#define CONVERSION_FRAME_SIZE (SOC_ADC_DIGI_DATA_BYTES_PER_CONV * ADC_CHANNEL_COUNT)
#define CONVERSION_POOL_SIZE CONVERSION_FRAME_SIZE * 1

#define BOOT_MODE_PIN GPIO_NUM_0
#define STORAGE_NAMESPACE "storage"

#define GPIO_SELECT_0 15
#define GPIO_SELECT_1 16
#define GPIO_SELECT_2 17
#define GPIO_SELECT_3 18
/*
 * Let's say, GPIO_SELECT_0=18, GPIO_SELECT_1=19
 * In binary representation,
 * 1ULL<<GPIO_SELECT_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_SELECT_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL             0000000000000000000011000000000000000000
 * */
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_SELECT_0) | (1ULL << GPIO_SELECT_1) | (1ULL << GPIO_SELECT_2) | (1ULL << GPIO_SELECT_3))

adc_continuous_handle_t adc_handle;

const uint32_t adc_channels[ADC_CHANNEL_COUNT] = { ADC_CHANNEL_2, ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_8 };
const uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = { GPIO_SELECT_0, GPIO_SELECT_1, GPIO_SELECT_2, GPIO_SELECT_3 };

// uint32_t adc_buffer[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT] = {0};

uint8_t current_amux_channel = 0;

static TaskHandle_t adc_task_handle;

static EventGroupHandle_t s_event_group;

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
    .pattern_num = ADC_CHANNEL_COUNT,
    .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_HIGH,
    .conv_mode = ADC_CONV_SINGLE_UNIT_1,
    .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
  };

  adc_digi_pattern_config_t adc_pattern[ADC_CHANNEL_COUNT] = { 0 };
  for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
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

  for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
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
    for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
      gpio_set_level(amux_select_pins[i], (current_amux_channel >> i) & 1);
    }

    for (uint8_t conversion_result_index = 0; conversion_result_index < conversion_frame_real_size; conversion_result_index += SOC_ADC_DIGI_DATA_BYTES_PER_CONV) {
      adc_digi_output_data_t *conversion_frame = (adc_digi_output_data_t *)&conversions[conversion_result_index];
      for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
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

static uint8_t rx_buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
/**
 * @brief Application Queue
 */
static QueueHandle_t app_queue;
typedef struct app_message {
  uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1]; // Data buffer
  size_t buf_len;                                 // Number of bytes received
  uint8_t itf;                                    // Index of CDC device interface
};

/**
 * @brief CDC device RX callback
 *
 * CDC device signals, that new data were received
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
  /* initialization */
  size_t rx_size = 0;

  /* read */
  esp_err_t ret = tinyusb_cdcacm_read(itf, rx_buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
  if (ret == ESP_OK) {

    struct app_message tx_msg = {
      .buf_len = rx_size,
      .itf = itf,
    };

    memcpy(tx_msg.buf, rx_buf, rx_size);
    xQueueSend(app_queue, &tx_msg, 0);
  } else {
    ESP_LOGE(TAG, "Read Error");
  }
}

/**
 * @brief CDC device line change callback
 *
 * CDC device signals, that the DTR, RTS states changed
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event) {
  int dtr = event->line_state_changed_data.dtr;
  int rts = event->line_state_changed_data.rts;
  ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

void tinyusb_init() {
  const tinyusb_config_t partial_init = {
    .device_descriptor = NULL,        // Use the default device descriptor specified in Menuconfig
    .string_descriptor = NULL,        // Use the default string descriptors specified in Menuconfig
    .configuration_descriptor = NULL, // Use the default configuration descriptor according to settings in Menuconfig
    .external_phy = false,            // Use internal USB PHY
  };

  const tinyusb_config_cdcacm_t acm_cfg = {
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .rx_unread_buf_sz = 64,
    .callback_rx = &tinyusb_cdc_rx_callback,
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
    .callback_line_coding_changed = NULL,
  };
  tusb_cdc_acm_init(&acm_cfg);
}

void cdc_task() {
  app_queue = xQueueCreate(5, sizeof(struct app_message));
  assert(app_queue);
  struct app_message msg;

  while (1) {
    if (xQueueReceive(app_queue, &msg, portMAX_DELAY)) {
      if (msg.buf_len) {

        /* Print received data*/
        ESP_LOGI(TAG, "Data from channel %d:", msg.itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

        /* write back */
        tinyusb_cdcacm_write_queue(msg.itf, msg.buf, msg.buf_len);
        esp_err_t err = tinyusb_cdcacm_write_flush(msg.itf, 0);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "CDC ACM write flush error: %s", esp_err_to_name(err));
        }
      }
    }
  }
}

void app_main(void) {
  esp_err_t err;

  // gpio_init();
  // adc_init();
  // mlev_init();

  // gpio_reset_pin(BOOT_MODE_PIN);
  // gpio_set_direction(BOOT_MODE_PIN, GPIO_MODE_INPUT);

  // while (1) {
  //   if (gpio_get_level(BOOT_MODE_PIN) == 0) {
  //     vTaskDelay(1000 / portTICK_PERIOD_MS);
  //     if (gpio_get_level(BOOT_MODE_PIN) == 0) {
  //       err = save_run_time();
  //       if (err != ESP_OK)
  //         printf("Error (%s) saving run time blob to NVS!\n", esp_err_to_name(err));
  //       printf("Restarting...\n");
  //       fflush(stdout);
  //       esp_restart();
  //     }
  //   }
  //   vTaskDelay(200 / portTICK_PERIOD_MS);
  // }
  // xTaskCreate(adc_task, "adc_task", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
  // xTaskCreate(mlev_task, "mlev_task", 2048, NULL, 5, NULL);
  // xTaskCreate(cdc_task, "cdc_task", 2048, NULL, 4, NULL);
  // xTaskCreate(debug_task, "debug_task", 2048, NULL, 5, NULL);
}
