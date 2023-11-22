#include "../src/rotary_encoder.h"
#include "pico/stdlib.h"
#include <stdio.h>

void volume_up() {
  printf("UP\n");
}

void volume_down() {
  printf("DOWN\n");
}

void play_pause() {
  printf("PLAY PAUSE\n");
}

void irq_callback(uint gpio, uint32_t events) {
  rotary_encoder_irq_callback(gpio, events, &volume_up, &volume_down, &play_pause);
}

int main(void) {
  stdio_init_all();

  init_rotary_encoder(&irq_callback);

  while (1) {
    sleep_ms(1);
  }

  return 0;
}