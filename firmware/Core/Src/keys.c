#include "keys.h"
#include "layout.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN_TIME_BETWEEN_EVENTS 10 // 10 ms
#define CALIBRATION_CYCLES 20

#define IDLE_VALUE_APPROX 1800
#define MAX_DISTANCE_APPROX 300
#define IDLE_VALUE_OFFSET 10
#define MAX_DISTANCE_OFFSET 40

#define ADC_CHANNEL_COUNT 5
#define AMUX_SELECT_PINS_COUNT 4
#define AMUX_CHANNEL_COUNT 16

extern ADC_HandleTypeDef hadc1;

uint32_t adc_channels[ADC_CHANNEL_COUNT] = {ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7};
uint32_t amux_select_pins[AMUX_SELECT_PINS_COUNT] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
ADC_ChannelConfTypeDef sConfig = {0};

static struct key keys[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT];

void keys_on_triggered(struct key *key);
void keys_on_reset(struct key *key);
void keys_on_change(struct key *key);
void noop() {
  return;
};

void init_key(uint8_t adc_channel, uint8_t amux_channel, uint8_t row, uint8_t col, uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  struct key *key = &keys[adc_channel][amux_channel];

  key->is_enabled = 1;

  key->row = row;
  key->column = col;

  key->calibration.is_calibrating = 1;
  key->calibration.cycles_count = 0;
  key->calibration.idle_value = IDLE_VALUE_APPROX;
  key->calibration.max_distance = MAX_DISTANCE_APPROX;

  key->actuation.is_reset = 1;
  key->actuation.changed_at = 0;
  key->actuation.trigger_offset = trigger_offset;
  key->actuation.rapid_trigger_offset = rapid_trigger_offset;
  key->actuation.is_continuous_rapid_trigger_enabled = is_continuous_rapid_trigger_enabled;
}

void keys_init(uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Rank = 1;

  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      struct key *key = &keys[adc_channel][amux_channel];
      key->is_enabled = 0;
    }
  }

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      if (channels_by_row_col[row][col][0] == XXXX || channels_by_row_col[row][col][1] == XXXX) {
        // no hall effect sensor on this channel
        continue;
      }

      init_key(channels_by_row_col[row][col][0], channels_by_row_col[row][col][1], row, col, trigger_offset, rapid_trigger_offset, is_continuous_rapid_trigger_enabled);
    }
  }
}

void update_key_state(struct key *key) {
  struct state state;

  // Get a reading
  state.value = HAL_ADC_GetValue(&hadc1);

  if (key->calibration.is_calibrating) {
    // Calibrate idle value
    key->calibration.idle_value = (1 - 0.6) * state.value + 0.6 * key->calibration.idle_value;

    key->calibration.cycles_count++;

    if (key->calibration.cycles_count >= CALIBRATION_CYCLES) {
      key->calibration.is_calibrating = 0;
    }
  }

  if (key->calibration.is_calibrating == 0) {
    // Calibrate idle value
    if (state.value > key->calibration.idle_value) {
      key->calibration.idle_value = state.value;
    }

    // Get distance from top
    if (state.value >= key->calibration.idle_value - IDLE_VALUE_OFFSET) {
      state.distance = 0;
    } else {
      state.distance = key->calibration.idle_value - state.value;
    }

    // Calibrate max distance value
    if (key->state.distance > key->calibration.max_distance) {
      key->calibration.max_distance = key->state.distance;
    }

    // Map distance in percentages
    // state.distance = 100 - (state.distance * 100 / (key->calibration.max_value - key->calibration.min_value));
    state.distance = (state.distance * 100 / key->calibration.max_distance);

    // Update velocity
    state.velocity = state.distance - key->state.distance;

    // Update Acceleration
    state.acceleration = (state.velocity - key->state.velocity) / 2;
    // state.acceleration = 0;

    // Update jerk
    state.jerk = (state.acceleration - key->state.acceleration) / 3;
    // state.jerk = 0;

    // Update movement
    state.is_pressing = key->state.velocity > 0 && state.velocity > 0;
  }

  key->state = state;
}

void update_key(struct key *key) {
  if (key->row == 0 && key->column == 0) {
    noop();
  }
  update_key_state(key);

  if (!key->calibration.is_calibrating) {
    /**
     * https://www.youtube.com/watch?v=_Sl-T6iQr8U&t
     *
     * |-----| <- FULL RESET      -
     * |     |                    | Continuous rapid trigger domain (deactivated when full_reset)
     * |     |                    |
     * |  -  | <- trigger_offset  -
     * |     |                    | Rapid trigger domain
     * |     |                    |
     * |-----|                    -
     *
     */

    uint32_t now = HAL_GetTick();
    uint8_t is_ready_to_trigger = key->actuation.triggered_at + MIN_TIME_BETWEEN_EVENTS < now;
    uint8_t has_moved_more_than_min = abs(key->state.distance - key->actuation.changed_at) > 10;
    uint8_t is_after_trigger_offset = key->state.distance > key->actuation.trigger_offset;

    // Trigger
    if (is_ready_to_trigger && key->actuation.is_reset && has_moved_more_than_min && is_after_trigger_offset) {
      key->actuation.is_reset = 0;
      key->actuation.triggered_at = now;
      key->actuation.changed_at = key->state.distance;
      // keys_on_triggered(key);
    }

    // Reset
    else if (!key->actuation.is_reset && has_moved_more_than_min && !is_after_trigger_offset) {
      key->actuation.changed_at = key->state.distance;
      key->actuation.is_reset = 1;
      // keys_on_reset(key);
    }

    // Full reset
    if (key->state.distance == 0) {
      key->actuation.changed_at = 0;
      // if (!key->actuation.is_reset) {
      //   keys_on_reset(key);
      // }
    }

    // Send event if motion detected
    if (is_ready_to_trigger && has_moved_more_than_min) {
      keys_on_change(key);
    }
  }
}

void keys_loop() {
  for (uint8_t amux_channel = 0; amux_channel < AMUX_CHANNEL_COUNT; amux_channel++) {
    for (uint8_t i = 0; i < AMUX_SELECT_PINS_COUNT; i++) {
      HAL_GPIO_WritePin(GPIOB, amux_select_pins[i], (amux_channel >> i) & 1);
    }

    for (uint8_t adc_channel = 0; adc_channel < ADC_CHANNEL_COUNT; adc_channel++) {
      if (keys[adc_channel][amux_channel].is_enabled == 0) {
        // no hall effect sensor on this channel
        continue;
      }

      sConfig.Channel = adc_channels[adc_channel];
      HAL_ADC_ConfigChannel(&hadc1, &sConfig);
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);
      HAL_ADC_Stop(&hadc1);

      update_key(&keys[adc_channel][amux_channel]);
    }
  }

  // sConfig.Channel = adc_channels[0];
  // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  // HAL_ADC_Start(&hadc1);
  // HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

  // update_key(&keys[0][0]);

  // HAL_ADC_Stop(&hadc1);
};
