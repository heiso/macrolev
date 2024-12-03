#include "tusb.h"
#include "usb_descriptors.h"
#include <hid.h>
#include <keyboard.h>
#include <stdlib.h>

extern uint8_t const desc_ms_os_20[];
extern struct key keyboard_keys[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT];
extern struct user_config keyboard_user_config;
extern uint32_t keyboard_last_cycle_duration;

static uint8_t should_send_consumer_report = 0;
static uint8_t should_send_keyboard_report = 0;

static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {0};
// static uint8_t is_screaming = 0;
static uint8_t consumer_report = 0;

CFG_TUSB_MEM_SECTION CFG_TUSB_MEM_ALIGN static uint8_t usb_vendor_control_buffer[400];

void hid_init() {
  tud_init(BOARD_TUD_RHPORT);
}

void hid_task() {
  tud_task();

  if ((should_send_consumer_report || should_send_keyboard_report) && tud_hid_ready()) {
    if (tud_suspended()) {
      tud_remote_wakeup();
    } else {
      if (should_send_consumer_report) {
        should_send_consumer_report = 0;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_report, 2);
      } else if (should_send_keyboard_report) {
        should_send_keyboard_report = 0;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
      }
    }
  }
}

void hid_press_key(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers |= key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == 0) {
        keycodes[i] = key->layers[layer].value;
        // if the key is violently pressed, automatically add the MAJ modifier :)
        // if (is_screaming) {
        //   is_screaming = 0;
        //   modifiers &= ~get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // } else if (i == 0 && key->state.velocity > keyboard_user_config.screaming_velocity_trigger) {
        //   is_screaming = 1;
        //   modifiers |= get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // }
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = key->layers[layer].value;
    should_send_consumer_report = 1;
    break;

  default:
    break;
  }
}

void hid_release_key(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers &= ~key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == key->layers[layer].value) {
        keycodes[i] = 0;
        // if (is_screaming) {
        //   is_screaming = 0;
        //   modifiers &= ~get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // }
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = 0;
    should_send_consumer_report = 1;
    break;

  default:
    break;
  }
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
  (void)instance;
  (void)protocol;

  // nothing to do since we use the same compatible boot report for both Boot and Report mode.
  // TODO set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len) {
  (void)instance;
  (void)report;
  (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;
  // if (instance == 1 && report_id == 0) {
  //   keyboard_write_config(buffer, bufsize);

  //   keyboard_init_keys();
  // }
}

// // Invoked when cdc when line state changed e.g connected/disconnected
// void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
//   (void)itf;

//   // connected
//   if (dtr && rts) {
//     // print initial message when connected
//     tud_cdc_write(&keyboard_user_config, 3);
//     tud_cdc_write_str('\r\n');
//     tud_cdc_write_flush();
//   }
// }

// // Invoked when CDC interface received data from host
// void tud_cdc_rx_cb(uint8_t itf) {
//   (void)itf;
// }

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
  switch (request->bmRequestType_bit.type) {
  case TUSB_REQ_TYPE_VENDOR: {
    switch (request->bRequest) {

    case VENDOR_REQUEST_CONFIG: {
      switch (request->wValue) {
      case VENDOR_VALUE_GET_LENGTH: {
        if (stage == CONTROL_STAGE_SETUP) {
          uint16_t size = sizeof(keyboard_user_config);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &keyboard_user_config, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_SET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, usb_vendor_control_buffer, request->wLength);
        } else if (stage == CONTROL_STAGE_DATA) {
          if (!keyboard_write_config(&usb_vendor_control_buffer, 0, request->wLength)) {
            return false;
          }
          keyboard_read_config();
          keyboard_init_keys();
        }

        break;
      }

      default:
        break;
      }
    }

    case VENDOR_REQUEST_RESET_CONFIG: {
      if (request->wValue == VENDOR_VALUE_SET) {
        if (stage == CONTROL_STAGE_SETUP) {
          if (!keyboard_write_config(&keyboard_default_user_config, 0, sizeof keyboard_default_user_config)) {
            return false;
          }
          keyboard_read_config();
          keyboard_init_keys();
          return tud_control_status(rhport, request);
        }

        break;
      }
    }

    case VENDOR_REQUEST_DFU_MODE: {
      if (request->wValue == VENDOR_VALUE_SET) {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_status(rhport, request);
        }

        break;
      }
    }

    case VENDOR_REQUEST_KEYS: {
      switch (request->wValue) {
      case VENDOR_VALUE_GET_LENGTH: {
        if (stage == CONTROL_STAGE_SETUP) {
          uint16_t size = sizeof(keyboard_keys);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &keyboard_keys, request->wLength);
        }

        break;
      }

      default:
        break;
      }
    }

    case VENDOR_REQUEST_CYCLE_DURATION: {
      switch (request->wValue) {
      case VENDOR_VALUE_GET_LENGTH: {
        if (stage == CONTROL_STAGE_SETUP) {
          uint16_t size = sizeof(keyboard_last_cycle_duration);
          return tud_control_xfer(rhport, request, &size, request->wLength);
        }

        break;
      }

      case VENDOR_VALUE_GET: {
        if (stage == CONTROL_STAGE_SETUP) {
          return tud_control_xfer(rhport, request, &keyboard_last_cycle_duration, request->wLength);
        }

        break;
      }

      default:
        break;
      }
    }

    case VENDOR_REQUEST_WEBUSB: {
      if (stage == CONTROL_STAGE_SETUP) {
        return tud_control_status(rhport, request);
      }

      break;
    }

    case VENDOR_REQUEST_MICROSOFT: {
      if (stage == CONTROL_STAGE_SETUP) {
        if (request->wIndex == 7) {
          // Get Microsoft OS 2.0 compatible descriptor
          uint16_t total_len;
          memcpy(&total_len, desc_ms_os_20 + 8, 2);

          return tud_control_xfer(rhport, request, (void *)(uintptr_t)desc_ms_os_20, total_len);
        }

        return false;
      }

      break;
    }

    default:
      break;
    }
  }

  case TUSB_REQ_TYPE_CLASS: {
    if (stage == CONTROL_STAGE_SETUP) {
      if (request->bRequest == 0x22) {
        // response with status OK
        return tud_control_status(rhport, request);
      }

      return false;
    }

    break;
  }

  default:
    break;
  }

  if (stage != CONTROL_STAGE_SETUP) {
    return true;
  }
  return false;
}