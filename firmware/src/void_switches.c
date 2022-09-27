#include "void_switches.h"
#include "config.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include <stdio.h>

uint8_t selected_adc;
/**
 * keydown_matrix length is rows * columns
 *
 * [a|b|c]
 * [d|e|f]
 *
 * 0b<fedcba>
 * x: 0 << columns * row + column
 * 0b000000
 *
 */
uint keydown_matrix;

void void_switches_on_keydown(uint8_t column, uint8_t row, uint index);
void void_switches_on_keyup(uint8_t column, uint8_t row, uint index);

uint get_index_from_pos(uint8_t column, uint8_t row) {
  return KEY_COLUMN_COUNT * row + column;
}

void init_state() {
  for (uint8_t column = 0; column < KEY_COLUMN_COUNT; column++) {
    for (uint8_t row = 0; row < KEY_ROW_COUNT; row++) {
      // all keys are up so set 0 everywhere
      keydown_matrix &= ~1 << get_index_from_pos(column, row);
    }
  }
}

void update_matrix(uint8_t column, uint8_t row, uint8_t value, uint8_t triggered_at) {
  uint index = get_index_from_pos(column, row);
  uint bitmask = 1 << index;

  // If key has passed the trigger point and is not already triggered
  // 110 & 100 -> 100 was triggered
  // 010 & 100 -> 000 was not
  if (value >= triggered_at && !(keydown_matrix & bitmask)) {
    // printf("[%d,%d] Pressed with %d ", column, row, value);
    // print_bytes(bitmask);
    // printf(" : ");
    // print_bytes(keydown_matrix);

    // Set keydown to 1
    keydown_matrix |= bitmask;
    void_switches_on_keydown(column, row, index);

    // printf(" -> ");
    // print_bytes(keydown_matrix);
    // printf("\n");
  }
  // If key is above trigger point
  else if (value < triggered_at && keydown_matrix & bitmask) {
    // printf("[%d,%d] Released with %d ", column, row, value);
    // print_bytes(bitmask);
    // printf(" : ");
    // print_bytes(keydown_matrix);

    // Set keydown to 0
    keydown_matrix &= ~bitmask;
    void_switches_on_keyup(column, row, index);

    // printf(" -> ");
    // print_bytes(keydown_matrix);
    // printf("\n");
  }
}

void __not_in_flash_func(adc_irq_handler)() {
  while (!adc_fifo_is_empty()) {
    uint16_t result = adc_fifo_get() * ADC_CONVERT;
    update_matrix(selected_adc, 1, result, 62);

    selected_adc = (selected_adc + 1) % KEY_COLUMN_COUNT;
  }
}

void void_switches_init() {
  init_state();

  for (uint8_t pin = PIN_FIRST_ADC; pin < KEY_COLUMN_COUNT; pin++) {
    adc_gpio_init(pin);
  }

  adc_init();
  adc_fifo_setup(
      true,  // Write each completed conversion to the sample FIFO
      true,  // Enable DMA data request (DREQ)
      1,     // DREQ (and IRQ) asserted when at least 1 sample present
      false, // We won't see the ERR bit because of 8 bit reads; disable.
      false  // Shift each sample to 8 bits when pushing to FIFO
  );

  // Divisor of 0 -> full speed. Free-running capture with the divider is
  // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
  // cycles (div not necessarily an integer). Each conversion takes 96
  // cycles, so in general you want a divider of 0 (hold down the button
  // continuously) or > 95 (take samples less frequently than 96 cycle
  // intervals). This is all timed by the 48 MHz ADC clock.
  adc_set_clkdiv(9600);

  irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
  adc_irq_set_enabled(true);
  irq_set_enabled(ADC_IRQ_FIFO, true);
  // irq_set_priority(ADC_IRQ_FIFO, PICO_HIGHEST_IRQ_PRIORITY);

  // 0b00000 5 adc (0, 1, 2, 3 and internal temp)
  uint8_t adc_mask = 0b00000;
  for (uint8_t i = 0; i < KEY_COLUMN_COUNT; i++) {
    adc_mask |= 1 << i;
  }

  adc_set_round_robin(adc_mask);
  adc_select_input(0);
  selected_adc = 0;
  adc_run(false);
  adc_fifo_drain();
  adc_run(true);
}

void print_bytes(int bytes) {
  for (int i = 12; i >= 0; i--) {
    if (1 << i & bytes) {
      printf("1");
    } else {
      printf("0");
    }
  }
}