#include "cJSON.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "macrolev.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "MACROLEV";

#define BOOT_MODE_PIN GPIO_NUM_0
#define STORAGE_NAMESPACE "storage"
#define JSON_FILENAME "config.json"

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

// USB CDC buffers and queue
static uint8_t rx_buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
static QueueHandle_t usb_queue;

typedef struct {
  uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
  size_t buf_len;
  uint8_t itf;
} usb_message_t;

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

// Initialize SPIFFS
static esp_err_t init_spiffs(void) {
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  esp_err_t ret = esp_vfs_spiffs_register(&conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    return ret;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    return ret;
  }

  ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  return ESP_OK;
}

// Save JSON to file
esp_err_t save_json_to_file(const char *filename, cJSON *json) {
  char *json_string = cJSON_PrintUnformatted(json);
  if (json_string == NULL) {
    ESP_LOGE(TAG, "Failed to print json");
    return ESP_FAIL;
  }

  char filepath[64];
  snprintf(filepath, sizeof(filepath), "/spiffs/%s", filename);

  FILE *f = fopen(filepath, "w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    free(json_string);
    return ESP_FAIL;
  }

  fprintf(f, "%s", json_string);
  fclose(f);
  free(json_string);

  ESP_LOGI(TAG, "JSON saved to file: %s", filepath);
  return ESP_OK;
}

// Load JSON from file
cJSON *load_json_from_file(const char *filename) {
  char filepath[64];
  snprintf(filepath, sizeof(filepath), "/spiffs/%s", filename);

  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return NULL;
  }

  // Get file size
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Read file content
  char *content = malloc(fsize + 1);
  if (content == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory");
    fclose(f);
    return NULL;
  }

  size_t read_size = fread(content, 1, fsize, f);
  content[read_size] = '\0';
  fclose(f);

  // Parse JSON
  cJSON *json = cJSON_Parse(content);
  free(content);

  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "Error parsing JSON before: %s", error_ptr);
    }
    return NULL;
  }

  ESP_LOGI(TAG, "JSON loaded from file: %s", filepath);
  return json;
}

/**
 * @brief CDC device RX callback
 *
 * CDC device signals, that new data were received
 *
 * @param[in] itf   CDC device index
 * @param[in] event CDC event type
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
  size_t rx_size = 0;
  esp_err_t ret = tinyusb_cdcacm_read(itf, rx_buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
  if (ret == ESP_OK) {
    // Null terminate the received data
    rx_buf[rx_size] = '\0';

    // Try to parse as JSON
    cJSON *json = cJSON_Parse((char *)rx_buf);
    if (json != NULL) {
      // Valid JSON received, save it
      ESP_LOGI(TAG, "Valid JSON received, saving to config.json");
      esp_err_t save_ret = save_json_to_file(JSON_FILENAME, json);

      // Send response
      const char *response;
      if (save_ret == ESP_OK) {
        response = "JSON saved successfully\r\n";
      } else {
        response = "Error saving JSON\r\n";
      }
      tinyusb_cdcacm_write_queue(itf, (uint8_t *)response, strlen(response));
      tinyusb_cdcacm_write_flush(itf, 0);

      cJSON_Delete(json);
    } else {
      // Not valid JSON, echo back as normal
      usb_message_t tx_msg = {
        .buf_len = rx_size,
        .itf = itf,
      };
      memcpy(tx_msg.buf, rx_buf, rx_size);
      xQueueSend(usb_queue, &tx_msg, 0);
    }
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

  // When DTR is set, terminal connection is opened
  if (dtr) {
    // Send welcome message
    const char *hello = "Hello! ESP32-S3 CDC device is ready!\r\n";
    tinyusb_cdcacm_write_queue(itf, (uint8_t *)hello, strlen(hello));
    tinyusb_cdcacm_write_flush(itf, 0);

    // Load and send current JSON content
    cJSON *json = load_json_from_file(JSON_FILENAME);
    if (json != NULL) {
      char *json_str = cJSON_PrintUnformatted(json);
      if (json_str != NULL) {
        tinyusb_cdcacm_write_queue(itf, (uint8_t *)json_str, strlen(json_str));
        tinyusb_cdcacm_write_queue(itf, (uint8_t *)"\r\n", 2);
        tinyusb_cdcacm_write_flush(itf, 0);
        free(json_str);
      }
      cJSON_Delete(json);
    } else {
      const char *no_json = "No JSON configuration found\r\n";
      tinyusb_cdcacm_write_queue(itf, (uint8_t *)no_json, strlen(no_json));
      tinyusb_cdcacm_write_flush(itf, 0);
    }
  }
}

void app_main(void) {
  // Initialize SPIFFS
  ESP_ERROR_CHECK(init_spiffs());

  // Create FreeRTOS primitives
  usb_queue = xQueueCreate(5, sizeof(usb_message_t));
  assert(usb_queue);
  usb_message_t msg;

  // Initialize TinyUSB
  ESP_LOGI(TAG, "USB initialization");
  const tinyusb_config_t tusb_cfg = {
    .device_descriptor = NULL,
    .string_descriptor = NULL,
    .external_phy = false,
    .configuration_descriptor = NULL,
  };
  ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

  // Initialize CDC
  tinyusb_config_cdcacm_t acm_cfg = {
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .callback_rx = &tinyusb_cdc_rx_callback,
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
    .callback_line_coding_changed = NULL
  };
  ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

  ESP_LOGI(TAG, "USB initialization DONE");

  // Main loop
  while (1) {
    if (xQueueReceive(usb_queue, &msg, portMAX_DELAY)) {
      if (msg.buf_len) {
        // Print received data
        ESP_LOGI(TAG, "Data from channel %d:", msg.itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

        // Echo back
        tinyusb_cdcacm_write_queue(msg.itf, msg.buf, msg.buf_len);
        esp_err_t err = tinyusb_cdcacm_write_flush(msg.itf, 0);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "CDC write error: %s", esp_err_to_name(err));
        }
      }
    }
  }
}
