#include "keyboard.h"
#include "DRV2605L.h"
#include <stdlib.h>

keyboard_user_config = {0};

const struct user_config default_user_config = {
    .trigger_offset = DEFAULT_TRIGGER_OFFSET,
    .reset_threshold = DEFAULT_RESET_THRESHOLD,
    .rapid_trigger_offset = DEFAULT_RAPID_TRIGGER_OFFSET,
    .screaming_velocity_trigger = DEFAULT_SCREAMING_VELOCITY_TRIGGER,
    .tap_timeout = DEFAULT_TAP_TIMEOUT,
    .keymaps = {
        // clang-format off
        [_BASE_LAYER] = {
            {HID_KEY_ESCAPE, HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE},
            {SPECIAL(HID_USAGE_CONSUMER_VOLUME_INCREMENT), HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_ENTER},
            {SPECIAL(HID_USAGE_CONSUMER_VOLUME_DECREMENT), HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_EUROPE_1, XXXX},
            {XXXX, HID_KEY_SHIFT_LEFT, HID_KEY_EUROPE_2, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, XXXX},
            {XXXX, HID_KEY_CONTROL_LEFT, HID_KEY_ALT_LEFT, HID_KEY_GUI_LEFT, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_GUI_RIGHT, HID_KEY_ALT_RIGHT, XXXX, HID_KEY_ARROW_LEFT, SPECIAL(HID_USAGE_CONSUMER_PLAY_PAUSE), HID_KEY_ARROW_RIGHT},
        },
        [_TAP_LAYER] = {
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
            {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, XXXX},
            {XXXX, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, HID_KEY_ARROW_UP, XXXX},
            {XXXX, ____, ____, ____, XXXX, ____, XXXX, ____, XXXX, ____, ____, XXXX, ____, HID_KEY_ARROW_DOWN, ____},
        },
        // clang-format on
    }};

// {adc_channel, amux_channel}
const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
    {{0, 10}, {0, 8}, {0, 7}, {0, 5}, {1, 9}, {1, 8}, {1, 6}, {2, 10}, {2, 9}, {2, 7}, {2, 6}, {3, 9}, {3, 8}, {3, 6}, {4, 2}},
    {{0, 11}, {0, 9}, {0, 6}, {0, 4}, {1, 10}, {1, 7}, {1, 5}, {2, 11}, {2, 8}, {2, 5}, {2, 4}, {3, 10}, {3, 7}, {3, 5}, {4, 1}},
    {{0, 12}, {0, 14}, {0, 2}, {1, 11}, {1, 14}, {1, 1}, {1, 4}, {2, 12}, {2, 15}, {2, 3}, {3, 11}, {3, 14}, {3, 1}, {3, 4}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 13}, {0, 0}, {0, 3}, {1, 13}, {1, 15}, {1, 2}, {1, 3}, {2, 14}, {2, 0}, {2, 2}, {3, 12}, {3, 15}, {3, 3}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 15}, {0, 1}, {1, 12}, {XXXX, XXXX}, {1, 0}, {XXXX, XXXX}, {2, 13}, {XXXX, XXXX}, {2, 1}, {3, 13}, {XXXX, XXXX}, {3, 0}, {3, 2}, {4, 0}},
};

static struct key keys[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT] = {0};

static uint8_t key_triggered = 0;

static uint8_t should_send_consumer_report = 0;
static uint8_t should_send_keyboard_report = 0;

static uint8_t modifiers = 0;
static uint8_t keycodes[6] = {0};
static uint8_t is_screaming = 0;
static uint8_t consumer_report = 0;

void keyboard_task() {
  key_triggered = 0;

  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    keyboard_select_amux(amux_channel);

    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      if (keys[adc_channel][amux_channel].is_enabled == 0) {
        continue;
      }
      keyboard_select_adc(adc_channel);

      update_key(&keys[adc_channel][amux_channel]);

      keyboard_close_adc();
    }
  }

  // If a key might be tap and a non tap key has been triggered, then the might be tap key is a normal trigger
  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      if (keys[adc_channel][amux_channel].is_enabled == 0 || keys[adc_channel][amux_channel].actuation.status != STATUS_MIGHT_BE_TAP) {
        continue;
      }

      struct key *key = &keys[adc_channel][amux_channel];
      uint8_t is_before_reset_offset = key->state.distance_8bits < key->actuation.reset_offset;
      uint8_t is_before_timeout = keyboard_get_time() - key->actuation.triggered_at <= keyboard_user_config.tap_timeout;

      // if might be tap, can be tap or triggered
      if (is_before_reset_offset && is_before_timeout) {
        key->actuation.status = STATUS_TAP;
        add_to_hid_report(key, _TAP_LAYER);
      } else if (!is_before_timeout || key_triggered) {
        key->actuation.status = STATUS_TRIGGERED;
        add_to_hid_report(key, _BASE_LAYER);
      }
    }
  }

  /**
   * @todo
   * @todo
   * @todo
   * @todo
   * split keycode and stuff in another file
   */
  if ((should_send_consumer_report || should_send_keyboard_report) && tud_hid_ready()) {
    if (tud_suspended()) {
      tud_remote_wakeup();
    } else {
      if (should_send_consumer_report) {
        should_send_consumer_report = 0;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_report, 2);
      } else if (should_send_keyboard_report) {
        should_send_keyboard_report = 0;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycodes);
      }
    }
  }
}

uint8_t get_bitmask_for_modifier(uint8_t keycode) {
  switch (keycode) {
  case HID_KEY_CONTROL_LEFT:
    return 0b00000001;
  case HID_KEY_SHIFT_LEFT:
    return 0b00000010;
  case HID_KEY_ALT_LEFT:
    return 0b00000100;
  case HID_KEY_GUI_LEFT:
    return 0b00001000;
  case HID_KEY_CONTROL_RIGHT:
    return 0b00010000;
  case HID_KEY_SHIFT_RIGHT:
    return 0b00100000;
  case HID_KEY_ALT_RIGHT:
    return 0b01000000;
  case HID_KEY_GUI_RIGHT:
    return 0b10000000;
  default:
    return 0b00000000;
  }
}

uint16_t get_usage_consumer_control(uint16_t value) {
  if (value > 0xFF) {
    return value & 0b0111111111111111;
  } else {
    return 0;
  }
}

void init_key(uint8_t adc_channel, uint8_t amux_channel, uint8_t row, uint8_t column) {
  struct key *key = &keys[adc_channel][amux_channel];

  key->is_enabled = 1;
  key->is_idle = 0;
  key->row = row;
  key->column = column;

  key->calibration.cycles_count = 0;
  key->calibration.idle_value = IDLE_VALUE_APPROX;
  key->calibration.max_distance = MAX_DISTANCE_APPROX;

  key->actuation.status = STATUS_RESET;
  key->actuation.trigger_offset = keyboard_user_config.trigger_offset;
  key->actuation.reset_offset = keyboard_user_config.trigger_offset - keyboard_user_config.reset_threshold;
  key->actuation.rapid_trigger_offset = keyboard_user_config.rapid_trigger_offset;
  key->actuation.is_continuous_rapid_trigger_enabled = 0;

  for (uint8_t i = 0; i < LAYERS_COUNT; i++) {
    if (keyboard_user_config.keymaps[i][row][column] != ____) {
      uint16_t usage_consumer_control = get_usage_consumer_control(keyboard_user_config.keymaps[i][row][column]);
      if (usage_consumer_control) {
        key->layers[i].type = KEY_TYPE_CONSUMER_CONTROL;
        key->layers[i].value = usage_consumer_control;
      } else {
        uint8_t bitmask = get_bitmask_for_modifier(keyboard_user_config.keymaps[i][row][column]);
        if (bitmask) {
          key->layers[i].type = KEY_TYPE_MODIFIER;
          key->layers[i].value = bitmask;
        } else {
          key->layers[i].type = KEY_TYPE_NORMAL;
          key->layers[i].value = keyboard_user_config.keymaps[i][row][column];
        }
      }
    }
  }
}

void keyboard_init_keys() {
  keyboard_read_config();

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (channels_by_row_col[row][col][0] != XXXX) {
        init_key(channels_by_row_col[row][col][0], channels_by_row_col[row][col][1], row, col);
      }
    }
  }
}

void add_to_hid_report(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers |= key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == 0) {
        keycodes[i] = key->layers[layer].value;
        // if the key is violently pressed, automatically add the MAJ modifier :)
        // if (is_screaming) {
        //   is_screaming = 0;
        //   modifiers &= ~get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // } else if (i == 0 && key->state.velocity > keyboard_user_config.screaming_velocity_trigger) {
        //   is_screaming = 1;
        //   modifiers |= get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // }
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = key->layers[layer].value;
    should_send_consumer_report = 1;
    break;
  }
}

void remove_from_hid_report(struct key *key, uint8_t layer) {
  switch (key->layers[layer].type) {
  case KEY_TYPE_MODIFIER:
    modifiers &= ~key->layers[layer].value;
    should_send_keyboard_report = 1;
    break;

  case KEY_TYPE_NORMAL:
    for (uint8_t i = 0; i < 6; i++) {
      if (keycodes[i] == key->layers[layer].value) {
        keycodes[i] = 0;
        // if (is_screaming) {
        //   is_screaming = 0;
        //   modifiers &= ~get_bitmask_for_modifier(HID_KEY_SHIFT_LEFT);
        // }
        should_send_keyboard_report = 1;
        break;
      }
    }
    break;

  case KEY_TYPE_CONSUMER_CONTROL:
    consumer_report = 0;
    should_send_consumer_report = 1;
    break;
  }
}

uint8_t update_key_state(struct key *key) {
  struct state state;

  // Get a reading
  state.value = keyboard_read_adc();

  if (key->calibration.cycles_count < CALIBRATION_CYCLES) {
    // Calibrate idle value
    float delta = 0.6;
    key->calibration.idle_value = (1 - delta) * state.value + delta * key->calibration.idle_value;
    key->calibration.cycles_count++;

    return 0;
  }

  // Calibrate idle value
  if (state.value > key->calibration.idle_value) {
    // opti possible sur float
    float delta = 0.8;
    key->calibration.idle_value = (1 - delta) * state.value + delta * key->calibration.idle_value;
    state.value = key->calibration.idle_value;
  }

  // Do nothing if key is idle
  if (key->state.distance == 0 && state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
    if (key->idle_counter >= IDLE_CYCLES_UNTIL_SLEEP) {
      key->is_idle = 1;
      return 0;
    }
    key->idle_counter++;
  }

  // Get distance from top
  if (state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
    state.distance = 0;
    key->actuation.direction_changed_point = 0;
  } else {
    state.distance = key->calibration.idle_value - IDLE_VALUE_OFFSET - state.value;
    key->is_idle = 0;
    key->idle_counter = 0;
  }

  // Calibrate max distance value
  if (state.distance > key->calibration.max_distance) {
    key->calibration.max_distance = state.distance;
  }

  // Limit max distance
  if (state.distance >= key->calibration.max_distance - MAX_DISTANCE_OFFSET) {
    state.distance = key->calibration.max_distance;
  }

  // Map distance in percentages
  state.distance_8bits = (state.distance * 0xff) / key->calibration.max_distance;

  float delta = 0.8;
  state.filtered_distance = (1 - delta) * state.distance_8bits + delta * key->state.filtered_distance;
  state.filtered_distance_8bits = state.filtered_distance;

  // Update velocity
  state.velocity = state.filtered_distance_8bits - key->state.filtered_distance_8bits;

  // Update direction
  if (key->state.velocity > 0 && state.velocity > 0 && key->actuation.direction != GOING_DOWN) {
    key->actuation.direction = GOING_DOWN;
    if (key->actuation.direction_changed_point != 0) {
      key->actuation.direction_changed_point = key->state.filtered_distance_8bits;
    }
  } else if (key->state.velocity < 0 && state.velocity < 0 && key->actuation.direction != GOING_UP) {
    key->actuation.direction = GOING_UP;
    if (key->actuation.direction_changed_point != 255) {
      key->actuation.direction_changed_point = key->state.filtered_distance_8bits;
    }
  }

  key->state = state;
  return 1;
}

void update_key_actuation(struct key *key) {
  /**
   * https://www.youtube.com/watch?v=_Sl-T6iQr8U&t
   *
   *                          -----   |--------|                           -
   *                            |     |        |                           |
   *    is_before_reset_offset  |     |        |                           |
   *                            |     |        |                           | Continuous rapid trigger domain (deactivated when full_reset)
   *                          -----   | ------ | <- reset_offset           |
   *                            |     |        |                           |
   *                          -----   | ------ | <- trigger_offset         -
   *                            |     |        |                           |
   *                            |     |        |                           |
   *   is_after_trigger_offset  |     |        |                           | Rapid trigger domain
   *                            |     |        |                           |
   *                            |     |        |                           |
   *                          -----   |--------|                           -
   *
   */

  // if rapid trigger enable, move trigger and reset offsets according to the distance taht began the trigger

  uint32_t now = keyboard_get_time();
  uint8_t is_after_trigger_offset = key->state.distance_8bits > key->actuation.trigger_offset;
  uint8_t is_before_reset_offset = key->state.distance_8bits < key->actuation.reset_offset;
  uint8_t has_rapid_trigger = key->actuation.rapid_trigger_offset != 0;
  uint8_t is_after_rapid_trigger_offset = key->state.distance_8bits > key->actuation.direction_changed_point - key->actuation.rapid_trigger_offset + keyboard_user_config.reset_threshold;
  uint8_t is_before_rapid_reset_offset = key->state.distance_8bits < key->actuation.direction_changed_point - key->actuation.rapid_trigger_offset;

  switch (key->actuation.status) {

  case STATUS_RESET:
    // if reset, can be triggered or tap
    if (is_after_trigger_offset) {
      if (key->layers[_TAP_LAYER].value) {
        key->actuation.status = STATUS_MIGHT_BE_TAP;
        // key_triggered = 1;
      } else {
        key->actuation.status = STATUS_TRIGGERED;
        key_triggered = 1;
        add_to_hid_report(key, _BASE_LAYER);
      }
      key->actuation.triggered_at = now;
    }
    break;

  case STATUS_RAPID_TRIGGER_RESET:
    if (!has_rapid_trigger) {
      key->actuation.status = STATUS_RESET;
      break;
    }
    // if reset, can be triggered or tap
    if (is_after_trigger_offset && key->actuation.direction == GOING_DOWN && is_after_rapid_trigger_offset) {
      if (key->layers[_TAP_LAYER].value) {
        key->actuation.status = STATUS_MIGHT_BE_TAP;
        key_triggered = 1;
      } else {
        key->actuation.status = STATUS_TRIGGERED;
        key_triggered = 1;
        add_to_hid_report(key, _BASE_LAYER);
      }
      key->actuation.triggered_at = now;
    } else if (is_before_reset_offset) {
      key->actuation.status = STATUS_RESET;
    }
    break;

  case STATUS_TAP:
    // if tap, can be reset
    key->actuation.status = STATUS_RESET;
    remove_from_hid_report(key, _TAP_LAYER);
    break;

  case STATUS_TRIGGERED:
    // if triggered, can be reset
    if (is_before_reset_offset) {
      key->actuation.status = STATUS_RESET;
      remove_from_hid_report(key, _BASE_LAYER);
    } else if (has_rapid_trigger && key->actuation.direction == GOING_UP && is_before_rapid_reset_offset) {
      key->actuation.status = STATUS_RAPID_TRIGGER_RESET;
      remove_from_hid_report(key, _BASE_LAYER);
    }
    break;
  }
}

void update_key(struct key *key) {
  if (!update_key_state(key)) {
    return;
  }

  update_key_actuation(key);
}
