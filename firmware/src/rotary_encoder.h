#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include "hardware/gpio.h"
#include "pin_config.h"

// 00 10 [11] 01
enum ENCODER_STATE {
  IDLE, // 11

  CW_1, // 01
  CW_2, // 00
  CW_3, // 10,

  CCW_1, // 10
  CCW_2, // 00
  CCW_3, // 01
};

void cw_callback(void);
void ccw_callback(void);
void sw_callback(void);
typedef void (*cw_callback_t)();
typedef void (*ccw_callback_t)();
typedef void (*sw_callback_t)();

void init_rotary_encoder(gpio_irq_callback_t irq_callback);
void rotary_encoder_irq_callback(uint gpio, uint32_t events, cw_callback_t cw_callback, ccw_callback_t ccw_callback, sw_callback_t sw_callback);

#endif