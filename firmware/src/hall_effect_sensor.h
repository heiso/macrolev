#ifndef HALL_EFFECT_SENSOR_H_
#define HALL_EFFECT_SENSOR_H_

#include "pin_config.h"
#include <stdio.h>

#define ADC_VREF 100
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT ADC_VREF / (ADC_RANGE - 1)

void init_hall_effect_sensor();
uint16_t hall_effect_sensor_read();

#endif