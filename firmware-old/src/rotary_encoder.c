#include "rotary_encoder.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <stdio.h>

static enum ENCODER_STATE encoder_state = IDLE;
unsigned long last_sw_trigger_time;
const int debounce_time = 250; // Delay for every push button may vary

void init_rotary_encoder(gpio_irq_callback_t irq_callback) {
  last_sw_trigger_time = to_ms_since_boot(get_absolute_time());

  gpio_init(PIN_ENC_SW);
  gpio_set_dir(PIN_ENC_SW, GPIO_IN);
  gpio_disable_pulls(PIN_ENC_SW);

  gpio_init(PIN_ENC_DT);
  gpio_set_dir(PIN_ENC_DT, GPIO_IN);
  gpio_disable_pulls(PIN_ENC_DT);

  gpio_init(PIN_ENC_CLK);
  gpio_set_dir(PIN_ENC_CLK, GPIO_IN);
  gpio_disable_pulls(PIN_ENC_CLK);

  gpio_set_irq_enabled_with_callback(PIN_ENC_SW, GPIO_IRQ_EDGE_FALL, true, irq_callback);
  gpio_set_irq_enabled(PIN_ENC_DT, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
  gpio_set_irq_enabled(PIN_ENC_CLK, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
}

void rotary_encoder_irq_callback(uint gpio, uint32_t events, cw_callback_t cw_callback, ccw_callback_t ccw_callback, sw_callback_t sw_callback) {
  if (gpio == PIN_ENC_CLK || gpio == PIN_ENC_DT) {
    bool clk = gpio_get(PIN_ENC_CLK);
    bool dt = gpio_get(PIN_ENC_DT);

    switch (encoder_state) {
    case IDLE: {
      if (!clk && dt) {
        encoder_state = CW_1;
      } else if (clk && !dt) {
        encoder_state = CCW_1;
      }
      break;
    }

    case CW_1: {
      if (!clk && !dt) {
        encoder_state = CW_2;
      } else if (clk && dt) {
        encoder_state = IDLE;
      }
      break;
    }

    case CW_2: {
      if (clk && !dt) {
        encoder_state = CW_3;
      } else if (clk && dt) {
        encoder_state = IDLE;
      }
      break;
    }

    case CW_3: {
      if (clk && dt) {
        encoder_state = IDLE;
        cw_callback();
      }
      break;
    }

    case CCW_1: {
      if (!clk && !dt) {
        encoder_state = CCW_2;
      } else if (clk && dt) {
        encoder_state = IDLE;
      }
      break;
    }

    case CCW_2: {
      if (!clk && dt) {
        encoder_state = CCW_3;
      } else if (clk && dt) {
        encoder_state = IDLE;
      }
      break;
    }

    case CCW_3: {
      if (clk && dt) {
        encoder_state = IDLE;
        ccw_callback();
      }
      break;
    }
    }
  }

  if (gpio == PIN_ENC_SW) {
    if ((to_ms_since_boot(get_absolute_time()) - last_sw_trigger_time) > debounce_time) {
      last_sw_trigger_time = to_ms_since_boot(get_absolute_time());
      sw_callback();
    }
  }
}