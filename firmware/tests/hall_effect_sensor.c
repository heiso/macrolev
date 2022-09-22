#include "../src/hall_effect_sensor.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main(void) {
  stdio_init_all();

  init_hall_effect_sensor();

  while (1) {
    printf("%d\n", hall_effect_sensor_read());
    sleep_ms(2);
  }

  return 0;
}