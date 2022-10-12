#include "../src/void_switches.h"
#include "../src/config.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main(void) {
  stdio_init_all();

  while (!stdio_usb_connected()) {
    sleep_ms(100);
  }

  void_switches_init(25, 0, 0);

  while (1) {
    void_switches_loop(true);
  }
  return 0;
}

void void_switches_on_triggered(struct void_switch *vswitch) {
  printf("%d-%d TRIGGERED at %d\n", vswitch->row, vswitch->column, vswitch->actuation.changed_at);
}

void void_switches_on_reset(struct void_switch *vswitch) {
  printf("%d-%d RESET at %d\n", vswitch->row, vswitch->column, vswitch->actuation.changed_at);
}

#define range 10
void void_switches_on_change(struct void_switch *vswitch) {
  // if (vswitch->row == 0 && vswitch->column == 0) {
  //   printf("\n");
  // }

  // uint8_t new_value = (vswitch->state.distance * range) / 100;

  // printf("%d,%d [", vswitch->row, vswitch->column);
  // for (int i = 0; i <= range; i++) {
  //   if (i <= new_value) {
  //     if (i >= range / 2) {
  //       printf("x");
  //     } else {
  //       printf("o");
  //     }
  //   } else {
  //     printf("-");
  //   }
  // }
  // printf("]  ");
}