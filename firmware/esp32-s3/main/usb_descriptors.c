#include "tinyusb.h"

// Interface definitions
enum {
  ITF_NUM_HID = 0,
  ITF_NUM_CDC,
  ITF_NUM_CDC_DATA,
  ITF_NUM_TOTAL
};

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN * CFG_TUD_HID + TUD_CDC_DESC_LEN * CFG_TUD_CDC)

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
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

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and 1 HID interface
 */
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