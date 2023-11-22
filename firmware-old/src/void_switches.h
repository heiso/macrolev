#ifndef VOID_SWITCHES_H_
#define VOID_SWITCHES_H_

#include "pico.h"
#include <stdint.h>

// #define DEBUG 1

struct calibration {
  uint8_t is_calibrating;
  uint16_t cycles_count;
  uint16_t min_value;
  uint8_t is_min_calibrated;
  uint16_t max_value;
  uint8_t is_max_calibrated;
};

struct state {
  int16_t value;
  int16_t distance;
  int16_t velocity;
  int16_t acceleration;
  int16_t jerk;
  uint8_t is_pressing;
};

enum actuation_status {
  RESET,
  TRIGGERED,
};

struct actuation {
  enum actuation_status status;
  uint16_t changed_at;
  uint16_t trigger_offset;
  uint16_t rapid_trigger_offset;
  uint8_t is_continuous_rapid_trigger_enabled;
  absolute_time_t trigger_timeout;
};

struct void_switch {
  uint8_t is_enabled;
  uint8_t row;
  uint8_t column;
  struct calibration calibration;
  struct state state;
  struct actuation actuation;
};

void void_switches_init(uint16_t trigger_point, uint16_t rapid_trigger_offset, uint8_t _continuous_rapid_trigger);

void void_switches_loop();

#endif