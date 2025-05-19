#include "cJSON.h"
#include "class/hid/hid_device.h"
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
#define CDC_ACCUM_BUF_SIZE (1024 * 1024) // 1MB buffer for large JSON payloads

#define MARKER "[EOF]"
#define MARKER_LEN (sizeof(MARKER) - 1)

static char cdc_accum_buf[CDC_ACCUM_BUF_SIZE];
static size_t cdc_accum_len = 0;

static uint8_t rx_buffer[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

static QueueHandle_t usb_cdc_rx_queue;

typedef struct usb_message {
  uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
  size_t buf_len;
} usb_message_t;

static void usb_cdc_rx_queue_handler_task(void *pvParameters) {
  usb_message_t msg;

  while (1) {
    if (xQueueReceive(usb_cdc_rx_queue, &msg, portMAX_DELAY) == pdTRUE) {
      ESP_LOGI(TAG, "Received %d bytes from USB", msg.buf_len);

      // Check for buffer overflow
      if (cdc_accum_len + msg.buf_len < CDC_ACCUM_BUF_SIZE) {
        memcpy(&cdc_accum_buf[cdc_accum_len], msg.buf, msg.buf_len);
        cdc_accum_len += msg.buf_len;

        // Search for marker
        if (cdc_accum_len >= MARKER_LEN) {
          for (size_t i = 0; i <= cdc_accum_len - MARKER_LEN; ++i) {
            if (memcmp(&cdc_accum_buf[i], MARKER, MARKER_LEN) == 0) {
              ESP_LOGI(TAG, "Marker detected ([EOF]) at position %d. Attempting to parse JSON.", (int)i);
              cdc_accum_buf[i] = '\0'; // Null-terminate before marker

              // Try to parse as JSON
              cJSON *json = cJSON_Parse(cdc_accum_buf);
              if (json != NULL) {
                ESP_LOGI(TAG, "Valid JSON received, saving to config.json");
                save_json_to_file(JSON_FILENAME, json);
                cJSON_Delete(json);
              } else {
                ESP_LOGE(TAG, "Invalid JSON received. Parse failed.");
              }

              // Move any data after the marker to the start of the buffer
              size_t remaining = cdc_accum_len - (i + MARKER_LEN);
              if (remaining > 0) {
                memmove(cdc_accum_buf, &cdc_accum_buf[i + MARKER_LEN], remaining);
              }
              cdc_accum_len = remaining;
              break;
            }
          }
        }
      } else {
        ESP_LOGE(TAG, "CDC accumulation buffer overflow. Resetting buffer.");
        cdc_accum_len = 0;
      }
    }
  }
}

// Initialize SPIFFS
static esp_err_t init_spiffs(void) {
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/",
    .partition_label = NULL,
    .max_files = 1,
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
  // snprintf(filepath, sizeof(filepath), "/%s", filename);
  snprintf(filepath, sizeof(filepath), filename);

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
  ESP_LOGI(TAG, "JSON: %s", cJSON_Print(json));
  return ESP_OK;
}

// Load JSON from file
cJSON *load_json_from_file(const char *filename) {
  char filepath[64];
  // snprintf(filepath, sizeof(filepath), "/%s", filename);
  snprintf(filepath, sizeof(filepath), filename);

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

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
  usb_message_t msg = { 0 };
  msg.buf_len = 0;

  // Read data from CDC
  esp_err_t ret = tinyusb_cdcacm_read(itf, msg.buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &msg.buf_len);
  if (ret == ESP_OK && msg.buf_len > 0) {
    // Send message to queue
    if (xQueueSend(usb_cdc_rx_queue, &msg, portMAX_DELAY) != pdTRUE) {
      ESP_LOGE(TAG, "Failed to send message to queue");
    }
  }
}

void config_storage_init(void) {
  // Initialize SPIFFS
  ESP_ERROR_CHECK(init_spiffs());

  // Create the CDC RX queue
  usb_cdc_rx_queue = xQueueCreate(5, sizeof(usb_message_t));
  assert(usb_cdc_rx_queue);

  // Create the CDC RX queue handler task
  xTaskCreate(usb_cdc_rx_queue_handler_task, "usb_cdc_rx_queue_handler", 4096, NULL, 5, NULL);

  // Initialize CDC
  tinyusb_config_cdcacm_t acm_cfg = {
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .callback_rx = &tinyusb_cdc_rx_callback,
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = NULL,
    .callback_line_coding_changed = NULL,
    .rx_unread_buf_sz = CONFIG_TINYUSB_CDC_RX_BUFSIZE
  };
  ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

  ESP_LOGI(TAG, "USB initialization DONE");
}