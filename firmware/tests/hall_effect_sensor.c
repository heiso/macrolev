// #include "../src/hall_effect_sensor.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define ADC_VREF 100
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT ADC_VREF / (ADC_RANGE - 1)
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
        printf("-------------------key DOWN %d\n", input);
      }
    } else {
      state = state & ~bitmask;
      reset_state = reset_state | bitmask;
    }
  }
}

int main(void) {
  stdio_init_all();

  adc_gpio_init(26);
  adc_gpio_init(27);
  // adc_gpio_init(28);

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
  adc_set_clkdiv(96000);

  irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
  adc_irq_set_enabled(true);
  irq_set_enabled(ADC_IRQ_FIFO, true);
  adc_set_round_robin(0b00011); // 0b00011
  adc_select_input(0);
  adc_run(false);
  adc_fifo_drain();
  adc_run(true);

  while (1) {
    for (int i = 4; i >= 0; i--) {
      if (state >> i & 1)
        printf("1");
      else
        printf("0");
    }
    printf("\n");
    sleep_ms(10);
  }

  return 0;
}