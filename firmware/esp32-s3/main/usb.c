#include "usb.h"
#include "class/hid/hid_device.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"

static const char *TAG = "MACROLEV";

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN * CFG_TUD_HID + TUD_CDC_DESC_LEN * CFG_TUD_CDC)

QueueHandle_t usb_cdc_rx_queue;

enum {
  ITF_NUM_HID = 0,
  ITF_NUM_CDC,
  ITF_NUM_CDC_DATA,
  ITF_NUM_TOTAL
};

const uint8_t hid_report_descriptor[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
};

const char *string_descriptor[] = {
  // array of pointer to string descriptors
  (char[]){ 0x09, 0x04 },                  // 0: is supported language is English (0x0409)
  CONFIG_TINYUSB_DESC_MANUFACTURER_STRING, // 1: Manufacturer
  CONFIG_TINYUSB_DESC_PRODUCT_STRING,      // 2: Product
  CONFIG_TINYUSB_DESC_SERIAL_STRING,       // 3: Serials, should use chip ID
  CONFIG_TINYUSB_DESC_CDC_STRING,          // 4: CDC Interface
  "Macrolev HID Device",                   // 5: HID Interface
};

static uint8_t configuration_descriptor[] = {
  // Configuration number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

  // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID, 5, HID_ITF_PROTOCOL_KEYBOARD, sizeof(hid_report_descriptor), 0x83, CFG_TUD_HID_EP_BUFSIZE, 10),

  // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
  TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, 0x81, 8, 0x02, 0x82, TUD_OPT_HIGH_SPEED ? 512 : 64),
};

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
  return hid_report_descriptor;
}

const tinyusb_config_t tusb_cfg = {
  .device_descriptor = NULL,
  .string_descriptor = string_descriptor,
  .external_phy = false,
  .configuration_descriptor = configuration_descriptor,
};

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
};

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
};

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

void usb_init(void) {
  // Initialize TinyUSB
  ESP_LOGI(TAG, "USB initialization");
  ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

  // Create the CDC RX queue
  usb_cdc_rx_queue = xQueueCreate(5, sizeof(usb_message_t));
  assert(usb_cdc_rx_queue);

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
};