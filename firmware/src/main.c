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

typedef struct report {
  REPORT_ID report_id;
  uint16_t value;
  bool consumed;
} Report;
static uint8_t read = 0;
static uint8_t write = 0;
#define REPORT_SIZE 14
static Report reports[REPORT_SIZE];
static bool last_report_complete = true;

void consume_report() {
  reports[read].consumed = true;
  read = (read + 1) % REPORT_SIZE;
}

void queue_report(REPORT_ID report_id, uint16_t value) {
  reports[write].report_id = report_id;
  reports[write].value = value;
  reports[write].consumed = false;

  write = (write + 1) % REPORT_SIZE;

  // if no room left, consume the oldest item in the queue
  if (write == read) {
    consume_report();
  }
}

bool has_reports() {
  return read != write;
}

void volume_down() {
  queue_report(REPORT_ID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
}

void volume_up() {
  queue_report(REPORT_ID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
}

void play_pause() {
  queue_report(REPORT_ID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_PLAY_PAUSE);
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
}

void press_key(uint8_t keycode) {
  queue_report(REPORT_ID_KEYBOARD, keycode);
}

void send_hid_report() {
  if (last_report_complete && has_reports()) {
    last_report_complete = false;

    if (has_reports()) {
      uint8_t index = read;
      consume_report();

      if (reports[index].value)

        switch (reports[index].report_id) {

        case REPORT_ID_CONSUMER_CONTROL: {
          tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &reports[index].value, sizeof(reports[index].value));
          if (reports[index].value != 0) {
            queue_report(REPORT_ID_CONSUMER_CONTROL, 0);
          }
        }

        case REPORT_ID_KEYBOARD: {
          uint8_t keycodes[6] = {0};
          keycodes[0] = reports[index].value;

          uint8_t i = 0;
          while (keycodes[i] != 0 && reports[read].consumed == false &&
                 reports[read].report_id == REPORT_ID_KEYBOARD &&
                 reports[read].value != 0 &&
                 i < 6) {
            i++;
            keycodes[i] = reports[read].value;
            consume_report();
          }

          tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycodes);

          if (keycodes[i] != 0) {
            queue_report(REPORT_ID_KEYBOARD, 0);
          }
        }
        }
    }
  }
}

void hid_task(void) {
  if (!tud_hid_ready())
    return;

  const uint32_t interval_ms = 2;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms)
    return;
  start_ms += interval_ms;

  // Remote wakeup
  if (tud_suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by hosta
    tud_remote_wakeup();
  } else {
    send_hid_report();
  }
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
  last_report_complete = true;
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

void void_switches_on_keydown(uint8_t column, uint8_t row, uint index) {
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
  uint8_t keycodes[6] = {0};
  keycodes[0] = KEYMAP[index];
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycodes);
}

void void_switches_on_keyup(uint8_t column, uint8_t row, uint index) {
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, 0);
}

int main(void) {
  board_init();
  init_rotary_encoder(&irq_callback);
  tusb_init();
  drv2605l_init();
  drv2605l_selectLibrary(1);
  drv2605l_setMode(DRV2605_MODE_INTTRIG);
  void_switches_init();

  while (1) {
    tud_task();
    hid_task();
  }
  return 0;
}