#ifndef __HID_H
#define __HID_H

#define VENDOR_REQUEST_KEYS 0xfe
#define VENDOR_REQUEST_CONFIG 0xff
#define VENDOR_REQUEST_RESET_CONFIG 0xfd
#define VENDOR_REQUEST_DFU_MODE 0xfc

#define VENDOR_VALUE_GET_LENGTH 0x00
#define VENDOR_VALUE_GET 0x01
#define VENDOR_VALUE_SET 0x02

void hid_init();

#endif /* __HID_H */