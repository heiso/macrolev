#pragma once

#include <stdint.h>

/**
 * MARK: Config
 */
struct switch_magnetic_profile {
  uint8_t id;
  uint16_t adc_reading_by_distance[255];
};

enum magnet_direction {
  NORTH_UP,
  NORTH_DOWN
};

struct deadzones {
  uint8_t start_offset;
  uint8_t end_offset;
};

struct rapid_trigger {
  uint8_t is_enabled;
  uint8_t is_continuous;
  uint8_t actuation_distance_delta;
  uint8_t release_distance_delta;
};

struct hardware {
  uint8_t adc_channel;
  uint8_t mux_channel;
  enum magnet_direction magnet_direction;
  uint8_t switch_magnetic_profile_id;
};

enum keycode_type {
  KEYCODE_TYPE_NORMAL,
  KEYCODE_TYPE_MODIFIER,
  KEYCODE_TYPE_CONSUMER_CONTROL,
  // For behaviors like reseting the keyboard, changing layer or anything firmware relevant
  KEYCODE_TYPE_SPECIAL,
};

struct keycode {
  enum keycode_type type;
  uint16_t value;
};

struct layer {
  // If this is > 0, wait for the key to be pressed for a hold_time_before_actuation time before sending the keycode
  uint16_t hold_time_before_actuation;

  // Print key in caps if the keypress velocity is more than the auto_caps_velocity treshold
  uint8_t is_auto_caps_enabled;
  uint8_t auto_caps_velocity_treshold;

  uint8_t keycodes_length;
  // one keypress could print multiple keycodes at the same time (like a macro)
  struct keycode keycodes[];
};

struct key_config {
  struct hardware hardware;

  struct deadzones deadzones;
  uint8_t actuation_distance;
  uint8_t release_distance;
  struct rapid_trigger rapid_trigger;

  uint8_t layers_length;
  struct layer layers[];
};

struct user_config {
  uint8_t key_configs_length;
  struct key_config key_configs[];
};

/**
 * MARK: Keys states
 *
 */
struct calibration {
  uint16_t idle_value;
  uint16_t max_distance;
};

enum direction {
  GOING_UP,
  GOING_DOWN,
};

struct key_state {
  uint8_t is_idle;

  struct calibration calibration;

  uint16_t raw_adc_value;
  uint16_t distance;
  uint8_t distance_8bits;
  int8_t velocity;
  // Distance from where the travel has begun
  uint8_t from;
  // Time since the travel has begun
  uint32_t since;
  enum direction direction;
};

/**
 * MARK: Functions
 */
void mlev_set_switch_value(uint8_t adc_channel, uint8_t amux_channel, uint32_t value);

void mlev_init();

void mlev_task();