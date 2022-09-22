#include "hall_effect_sensor.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include <stdio.h>

void init_hall_effect_sensor() {
  adc_init();
  adc_gpio_init(PIN_HALL_EFFECT_SENSOR);
  adc_select_input(ADC_HALL_EFFECT_SENSOR);
}

uint16_t hall_effect_sensor_read() {
  return adc_read() * ADC_CONVERT;
}