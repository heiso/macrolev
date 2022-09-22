/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// 0
// 1       @
// 2
// 3             @
// 4
// 5
// 6
// 7
//
// E.g. if slave addresses 0x12 and 0x34 were acknowledged.

#include "../src/DRV2605L.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

int main() {
  // Enable UART so we can print status output
  stdio_init_all();

  printf("waiting for usb host");
  while (!tud_cdc_connected()) {
    printf(".");
    sleep_ms(500);
  }
  printf("\nusb host detected!\n");

  drv2605l_init();

  drv2605l_selectLibrary(1);
  drv2605l_setMode(DRV2605_MODE_INTTRIG);

  uint8_t effect = 1;
  while (1) {
    printf("%d\n", effect);
    drv2605l_setWaveform(0, 1);
    drv2605l_setWaveform(1, 0);
    drv2605l_go();

    sleep_ms(500);

    effect++;
    if (effect > 117)
      effect = 1;
  }

  return 0;
}
