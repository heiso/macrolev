#ifndef __KEYS_H
#define __KEYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

struct calibration {
  uint8_t is_calibrating;
  uint16_t cycles_count;
  uint16_t idle_value;
  uint16_t max_distance;
};

struct state {
  int16_t value;
  int16_t distance;
  int16_t distance_percentage;
  int16_t velocity;
  int16_t acceleration;
  int16_t jerk;
  uint8_t is_pressing;
};

enum actuation_status {
  STATUS_RESET,
  STATUS_TRIGGERED
};

struct actuation {
  uint8_t is_reset;
  uint16_t changed_at;
  uint16_t trigger_offset;
  uint16_t rapid_trigger_offset;
  uint8_t is_continuous_rapid_trigger_enabled;
  uint32_t triggered_at;
};

struct key {
  uint8_t is_enabled;
  uint8_t row;
  uint8_t column;
  struct calibration calibration;
  struct state state;
  struct actuation actuation;
};

void keys_init(uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled);

void keys_loop();

#ifdef __cplusplus
}
#endif

#endif /* __KEYS_H */