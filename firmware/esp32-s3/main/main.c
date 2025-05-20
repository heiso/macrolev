#include "config_storage.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "usb.h"
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

#define BOOT_MODE_PIN GPIO_NUM_0

void app_main(void) {
  usb_init();
  config_storage_init();

  // Main loop
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(10));
    // if (xQueueReceive(usb_queue, &msg, portMAX_DELAY)) {
    //   if (msg.buf_len) {
    //     // Print received data
    //     ESP_LOGI(TAG, "Data from channel %d with length %d", msg.itf, msg.buf_len);
    //     ESP_LOG_BUFFER_HEXDUMP(TAG, msg.buf, msg.buf_len, ESP_LOG_INFO);

    //     // // Echo back
    //     // tinyusb_cdcacm_write_queue(msg.itf, msg.buf, msg.buf_len);
    //     // esp_err_t err = tinyusb_cdcacm_write_flush(msg.itf, 0);
    //     // if (err != ESP_OK) {
    //     //   ESP_LOGE(TAG, "CDC write error: %s", esp_err_to_name(err));
    //     // }
    //   }
    // }
  }
}