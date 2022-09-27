#include "bsp/board.h"

#include "DRV2605L.h"
#include "hall_effect_sensor.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "rotary_encoder.h"
#include "stdlib.h"
#include "usb_descriptors.h"

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
  drv2605l_setWaveform(0, 1);
  drv2605l_setWaveform(1, 0);
  drv2605l_go();
}

void send_hid_report() {
  if (last_report_complete && has_reports()) {
    last_report_complete = false;

    if (has_reports()) {
      uint8_t index = read;
      consume_report();

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

#define ADC_COUNT 2
#define BUFF_SIZE 100 * ADC_COUNT - 1

uint16_t buffer[BUFF_SIZE];
uint16_t head = 0;
uint8_t state = 0b0000;
uint8_t reset_state = 0b1111;

void __not_in_flash_func(adc_irq_handler)() {
  while (!adc_fifo_is_empty()) {
    uint16_t result = adc_fifo_get() * ADC_CONVERT;
    buffer[head] = result;
    if (head == BUFF_SIZE) {
      head = 0;
    } else {
      head++;
    }

    uint8_t input = head % ADC_COUNT;
    uint8_t bitmask = 1 << input;

    if (buffer[head] > 60) {
      state = state | bitmask;
      if (state & bitmask && reset_state & bitmask) {
        reset_state = reset_state & ~bitmask;
        press_key(0x04 + input);
      }
    } else {
      state = state & ~bitmask;
      reset_state = reset_state | bitmask;
    }
  }
}

int main(void) {
  board_init();
  init_rotary_encoder(&irq_callback);
  tusb_init();
  drv2605l_init();
  drv2605l_selectLibrary(1);
  drv2605l_setMode(DRV2605_MODE_INTTRIG);

  adc_gpio_init(26);
  adc_gpio_init(27);

  adc_init();
  adc_fifo_setup(
      true,  // Write each completed conversion to the sample FIFO
      true,  // Enable DMA data request (DREQ)
      1,     // DREQ (and IRQ) asserted when at least 1 sample present
      false, // We won't see the ERR bit because of 8 bit reads; disable.
      false  // Shift each sample to 8 bits when pushing to FIFO
  );

  adc_set_clkdiv(96000);

  irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
  adc_irq_set_enabled(true);
  irq_set_enabled(ADC_IRQ_FIFO, true);
  // // irq_set_priority(ADC_IRQ_FIFO, PICO_HIGHEST_IRQ_PRIORITY);
  adc_set_round_robin(0b00011);
  adc_select_input(0);
  adc_run(false);
  adc_fifo_drain();
  adc_run(true);

  while (1) {
    tud_task();
    hid_task();
  }
  return 0;
}