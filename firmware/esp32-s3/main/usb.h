#include "tinyusb.h"

typedef struct usb_message {
  uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
  size_t buf_len;
} usb_message_t;

void usb_init(void);
