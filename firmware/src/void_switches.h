#ifndef VOID_SWITCHES_H_
#define VOID_SWITCHES_H_

#include "pico/stdlib.h"

#define ADC_VREF 100
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT ADC_VREF / (ADC_RANGE - 1)

void void_switches_init();
void void_switches_calibration();
void void_switches_loop();
uint16_t void_switches_get_mapped_value(uint8_t row, uint8_t column, uint16_t max);

#endif