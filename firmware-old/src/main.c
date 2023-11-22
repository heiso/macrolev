#include "DRV2605L.h"
#include "bsp/board.h"
#include "config.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "rotary_encoder.h"
#include "stdlib.h"
#include "usb_descriptors.h"
#include "void_switches.h"

/**
 * MUST HAVE
 * - understand how keyup hid report should be sent, we might keep the same order
 * - consumer descriptor
 * - keyboard descriptor
 * - keymap and macro (multiple keycodes in one keypress)
 *
 * NICE TO HAVE
 * - rewrite in rust
 * - set keymap trough usb
 * - NKRO and custom hid descriptor
 *
 * NOTES
 * https://github.com/qmk/qmk_firmware/blob/master/docs/usb_nkro.txt
 * https://www.devever.net/~hl/usbnkro
 * https://www.microsoft.com/applied-sciences/projects/anti-ghosting-demo
 * https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/
 *
 *
 * usbhid-dump
 */

static bool can_send_report;
static bool report_sent;
static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE};
static uint16_t consumer_report = HID_KEY_NONE;
static REPORT_ID current_report_id = REPORT_ID_KEYBOARD;

#define SET_BIT(BITFIELD, INDEX) BITFIELD |= ((uint8_t)1 << INDEX)
#define CLR_BIT(BITFIELD, INDEX) BITFIELD &= ~((uint8_t)1 << INDEX)
#define IS_BIT_SET(BITFIELD, INDEX) ((BITFIELD >> INDEX) & 1)

void volume_down() {
  consumer_report = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
  report_sent = false;
}

void volume_up() {
  consumer_report = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
  report_sent = false;
}

void play_pause() {
  consumer_report = HID_USAGE_CONSUMER_PLAY_PAUSE;
  report_sent = false;
}

void irq_callback(uint gpio, uint32_t events) {
  rotary_encoder_irq_callback(gpio, events, &volume_up, &volume_down, &play_pause);
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len) {
  current_report_id = current_report_id == REPORT_ID_KEYBOARD ? REPORT_ID_CONSUMER_CONTROL : REPORT_ID_KEYBOARD;
  can_send_report = true;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  return reqlen;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
}

void void_switches_on_triggered(struct void_switch *vswitch) {
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
  if (KEYMAP[vswitch->row][vswitch->column] != HID_KEY_NONE) {
    uint8_t index = vswitch->row * COLUMNS + vswitch->column;
    keycodes[index] = KEYMAP[vswitch->row][vswitch->column];
    modifiers |= KEYMAP_MODIFIERS[vswitch->row][vswitch->column];
    report_sent = false;
  } else if (KEYMAP_CONSUMER[vswitch->row][vswitch->column]) {
    consumer_report = KEYMAP_CONSUMER[vswitch->row][vswitch->column];
    report_sent = false;
  }
}

void void_switches_on_reset(struct void_switch *vswitch) {
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
  if (KEYMAP[vswitch->row][vswitch->column] != HID_KEY_NONE) {
    uint8_t index = vswitch->row * COLUMNS + vswitch->column;
    modifiers &= ~(KEYMAP_MODIFIERS[vswitch->row][vswitch->column]);
    keycodes[index] = HID_KEY_NONE;
    report_sent = false;
  } else if (KEYMAP_CONSUMER[vswitch->row][vswitch->column]) {
    consumer_report = HID_KEY_NONE;
    report_sent = false;
  }
}

void void_switches_on_change(struct void_switch *vswitch) {}

int main() {
  board_init();
  // init_rotary_encoder(&irq_callback);
  tusb_init();
  drv2605l_init();
  drv2605l_selectLibrary(1);
  drv2605l_setMode(DRV2605_MODE_INTTRIG);
  void_switches_init(25, 0, 0);

  can_send_report = true;
  report_sent = true;

  while (1) {
    tud_task();
    void_switches_loop();

    if (tud_hid_ready() && !report_sent && can_send_report) {
      if (tud_suspended()) {
        tud_remote_wakeup();
      } else {
        can_send_report = false;
        if (current_report_id == REPORT_ID_KEYBOARD) {
          tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
        } else {
          tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_report, sizeof(consumer_report));
          report_sent = true;
        }
      }
    }
  }
  return 0;
}