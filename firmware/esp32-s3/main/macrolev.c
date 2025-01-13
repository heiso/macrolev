#include <macrolev.h>
#include <stdint.h>

struct keycode default_keycode = {
  .type = KEYCODE_TYPE_NORMAL,
  .value = 0xE7, // right
};

struct key_config default_key_config = {
  .hardware = {
      .adc_channel = 0,
      .mux_channel = 0,
      .magnet_direction = NORTH_UP,
      .switch_magnetic_profile_id = 0,
  },
  .deadzones = {
      .start_offset = 0,
      .end_offset = 0,
  },
  .actuation_distance = 128,
  .release_distance = 50,
  .rapid_trigger = {
      .is_enabled = 1,
      .is_continuous = 1,
      .actuation_distance_delta = 50,
      .release_distance_delta = 50,
  },
  .layers_length = 1,
  .layers = { {
      .hold_time_before_actuation = 0,
      .is_auto_caps_enabled = 0,
      .auto_caps_velocity_treshold = 0,
      .keycodes_length = 1,
  } }
};

struct user_config config;

uint8_t is_calibrating;

// store it into a matrix ??
// store it into a matrix ??
// store it into a matrix ??
// store it into a matrix ??
// store it into a matrix ??
// would be easier to acces a key from it's adc_channel and amux
struct key_state key_states[];

void init_config() {
  // read adc_channels and amux_channels from config
  // read adc_channels and amux_channels from config
  // read adc_channels and amux_channels from config
  // read adc_channels and amux_channels from config
  // read adc_channels and amux_channels from config
  // store counts somewhere
  config.key_configs_length = 1;

  for (uint8_t i = 0; i < config.key_configs_length; i++) {
    config.key_configs[i] = default_key_config;
    config.key_configs[i].layers[0].keycodes[0] = default_keycode;
  }
};

void init_key_states() {
  for (uint8_t i = 0; i < config.key_configs_length; i++) {
    key_states[i].is_idle = 0;
    key_states[i].calibration.idle_value = 1800;
    key_states[i].calibration.max_distance = 500;
    key_states[i].raw_adc_value = 0;
    key_states[i].distance = 0;
    key_states[i].distance_8bits = 0;
    key_states[i].velocity = 0;
    key_states[i].from = 0;
    key_states[i].since = 0;
    key_states[i].direction = GOING_DOWN;
  }
};

void mlev_set_switch_value(uint8_t adc_channel, uint8_t amux_channel, uint32_t value) {

};

void mlev_init() {
  // check config present
  // check config validity (version ?)
  // calibrate switches
};

void mlev_task() {
  // tud_task
  // update keys states
  // send hid reports
};
