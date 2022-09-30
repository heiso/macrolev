#include "../src/void_switches.h"
#include "../src/config.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main(void) {
  stdio_init_all();

  void_switches_init();
  void_switches_calibration();

  while (1) {
    uint32_t start = time_us_32();

    void_switches_loop();

    for (uint8_t row = 0; row < ROWS; row++) {
      for (uint8_t column = 0; column < COLUMNS; column++) {
        uint8_t range = 10;
        uint8_t travel = void_switches_get_mapped_value(row, column, range);
        printf("%d,%d [", row, column);
        for (int i = 0; i <= range; i++) {
          if (i <= travel) {
            if (i >= range / 2) {
              printf("x");
            } else {
              printf("o");
            }
          } else {
            printf("-");
          }
        }
        printf("]  ");
      }
    }

    uint32_t duration = time_us_32() - start;
    printf(" -> %dus\n", duration);
  }
  return 0;
}

void void_switches_on_keydown(uint8_t column, uint8_t row, uint index) {
  printf("DOWN %d, %d\n", column, row);
}

void void_switches_on_keyup(uint8_t column, uint8_t row, uint index) {
  printf("UP %d, %d\n", column, row);
}