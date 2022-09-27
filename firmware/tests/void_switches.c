#include "../src/void_switches.h"
#include "../src/config.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main(void) {
  stdio_init_all();

  sleep_ms(5000);

  void_switches_init();

  while (1) {
    sleep_ms(10);
  }

  return 0;
}

void void_switches_on_keydown(uint8_t column, uint8_t row, uint index) {
  printf("DOWN %d, %d", column, row);
  printf("%d\n", KEYMAP[index]);
}

void void_switches_on_keyup(uint8_t column, uint8_t row, uint index) {
  printf("UP %d, %d\n", column, row);
}