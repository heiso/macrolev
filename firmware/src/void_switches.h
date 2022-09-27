#ifndef VOID_SWITCHES_H_
#define VOID_SWITCHES_H_

#define ADC_VREF 100
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT ADC_VREF / (ADC_RANGE - 1)

void void_switches_init();

#endif