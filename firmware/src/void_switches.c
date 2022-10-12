#include "void_switches.h"
#include "config.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_TIME_BETWEEN_EVENTS_US 10000 // 10ms
#define CONSTANT_LOOP_DURATION_US 50
#define CALIBRATION_CYCLES 1000

#define MIN_VALUE_APPROX 400
#define MAX_VALUE_APPROX 800
#define MIN_OFFSET 10
#define MAX_OFFSET 40

static struct void_switch vswitches[ROWS][COLUMNS];

void void_switches_on_triggered(struct void_switch *vswitch);
void void_switches_on_reset(struct void_switch *vswitch);
void void_switches_on_change(struct void_switch *vswitch);

void init_switch(uint8_t row, uint8_t column, uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  struct void_switch *vswitch = &vswitches[row][column];

  vswitch->row = row;
  vswitch->column = column;

  vswitch->is_enabled = 0;

  vswitch->calibration.is_calibrating = 1;
  vswitch->calibration.cycles_count = 0;
  vswitch->calibration.min_value = MIN_VALUE_APPROX;
  vswitch->calibration.is_min_calibrated = 0;
  vswitch->calibration.max_value = MAX_VALUE_APPROX;
  vswitch->calibration.is_max_calibrated = 0;

  vswitch->actuation.status = RESET;
  vswitch->actuation.changed_at = 0;
  vswitch->actuation.trigger_offset = trigger_offset;
  vswitch->actuation.rapid_trigger_offset = rapid_trigger_offset;
  vswitch->actuation.is_continuous_rapid_trigger_enabled = is_continuous_rapid_trigger_enabled;

  vswitch->state.is_pressing = 0;
}

void update_switch_state(struct void_switch *vswitch) {
  struct state state;

  // Get a reading
  state.value = adc_read();

  // Get distance
  if (state.value <= vswitch->calibration.min_value) {
    state.distance = 0;
  } else if (state.value >= vswitch->calibration.max_value) {
    state.distance = vswitch->calibration.max_value - vswitch->calibration.min_value;
  } else {
    state.distance = state.value - vswitch->calibration.min_value;
  }

  // Update velocity
  state.velocity = state.distance - vswitch->state.distance;

  // // Update Acceleration
  // state.acceleration = (state.velocity - vswitch->state.velocity) / 2;

  // // Update jerk
  // state.jerk = (state.acceleration - vswitch->state.acceleration) / 3;

  // Map distance in percentages
  state.distance = state.distance * 100 / (vswitch->calibration.max_value - vswitch->calibration.min_value);

  // Update movement
  state.is_pressing = vswitch->state.velocity > 0 && state.velocity > 0;

  vswitch->state = state;
}

void update_switch(struct void_switch *vswitch) {
  if (vswitch->calibration.is_calibrating) {
    update_switch_state(vswitch);

    // Calibrate min value
    vswitch->calibration.min_value = (1 - 0.9) * vswitch->state.value + 0.9 * vswitch->calibration.min_value;

    vswitch->calibration.cycles_count++;

    if (vswitch->calibration.cycles_count >= CALIBRATION_CYCLES) {
      vswitch->calibration.is_calibrating = 0;
      vswitch->calibration.min_value = vswitch->calibration.min_value + MIN_OFFSET;

      // TODO: Find a better way to detect if a switch is out of order
      if (vswitch->calibration.min_value > MIN_VALUE_APPROX * 0.85) {
        vswitch->calibration.is_min_calibrated = 1;
      }
    }
  }

  else if (!vswitch->calibration.is_max_calibrated) {
    update_switch_state(vswitch);

    // Calibrate max value
    // Add some brain, if we never reach max value in multiple pressing/releasing cycles, might change max value
    if (vswitch->state.value - MAX_OFFSET > vswitch->calibration.max_value) {
      vswitch->calibration.max_value = vswitch->state.value - MAX_OFFSET;
    }

    // TODO: Find a better way to detect if a switch is out of order
    if (vswitch->calibration.max_value > vswitch->calibration.min_value + 100) {
      vswitch->calibration.is_max_calibrated = 1;
      vswitch->is_enabled = 1;
    }
  }

  else if (vswitch->is_enabled) {
    update_switch_state(vswitch);

    // Calibrate max value
    // Add some brain, if we never reach max value in multiple pressing/releasing cycles, might change max value
    if (vswitch->state.value - MAX_OFFSET > vswitch->calibration.max_value) {
      vswitch->calibration.max_value = vswitch->state.value - MAX_OFFSET;
    }

    // Send event if motion detected
    if (vswitch->state.velocity != 0) {
      void_switches_on_change(vswitch);
    }

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

    bool is_ready_to_trigger = time_reached(vswitch->actuation.trigger_timeout);
    bool is_reset = vswitch->actuation.status == RESET;
    bool has_moved_more_than_min = abs(vswitch->state.distance - vswitch->actuation.changed_at) > 10;
    bool is_after_trigger_offset = vswitch->state.distance > vswitch->actuation.trigger_offset;

    // Trigger
    if (is_ready_to_trigger && is_reset && has_moved_more_than_min && is_after_trigger_offset) {
      vswitch->actuation.status = TRIGGERED;
      vswitch->actuation.trigger_timeout = make_timeout_time_us(MIN_TIME_BETWEEN_EVENTS_US);
      vswitch->actuation.changed_at = vswitch->state.distance;
      void_switches_on_triggered(vswitch);
    }

    // Reset
    else if (!is_reset && has_moved_more_than_min && !is_after_trigger_offset) {
      vswitch->actuation.status = RESET;
      vswitch->actuation.changed_at = vswitch->state.distance;
      void_switches_on_reset(vswitch);
    }

    // Full reset
    if (vswitch->state.distance == 0) {
      vswitch->actuation.changed_at = 0;
      if (!is_reset) {
        vswitch->actuation.status = RESET;
        void_switches_on_reset(vswitch);
      }
    }
  }
}

/**
 * @brief Init gpios and adc
 *
 * @param trigger_point Trigger point, when the switch value is or is greater than the trigger point, fire an event.
 */
void void_switches_init(uint16_t trigger_offset, uint16_t rapid_trigger_offset, uint8_t is_continuous_rapid_trigger_enabled) {
  for (uint8_t row = 0; row < ROWS; row++) {
    for (uint8_t column = 0; column < COLUMNS; column++) {
      init_switch(row, column, trigger_offset, rapid_trigger_offset, is_continuous_rapid_trigger_enabled);
    }
  }

  // Init pins for columns and rows
  for (uint8_t i = 0; i < COLUMNS; i++) {
    adc_gpio_init(PIN_FIRST_COLUMN + i);
  }
  for (uint8_t i = 0; i < ROWS; i++) {
    gpio_init(PIN_FIRST_ROW + i);
    gpio_set_dir(PIN_FIRST_ROW + i, GPIO_OUT);
    gpio_disable_pulls(PIN_FIRST_ROW + i);
  }

  // Init adc
  adc_init();

  sleep_ms(100);
}

/**
 * @brief Main loop
 *
 */
void void_switches_loop() {
#ifdef DEBUG
  uint32_t start = time_us_32();
#endif

  absolute_time_t end = make_timeout_time_us(CONSTANT_LOOP_DURATION_US);

  for (uint8_t row = 0; row < ROWS; row++) {
    gpio_put(PIN_FIRST_ROW + row, 1);

    for (uint8_t column = 0; column < COLUMNS; column++) {
      adc_select_input(column);
      // Wait a bit before reading adc, being to fast could result in "some" spikes.
      sleep_us(1);

      update_switch(&vswitches[row][column]);
    }
    gpio_put(PIN_FIRST_ROW + row, 0);
  }

#ifdef DEBUG
  uint32_t loop_duration_before_wait = time_us_32() - start;
#endif

  sleep_until(end);

#ifdef DEBUG
  uint32_t loop_duration = time_us_32() - start;
  struct void_switch *vswitch = &vswitches[1][1];
  printf("/*");
  // Misc values
  printf("%d,", vswitch->calibration.is_calibrating);
  printf("%d,", vswitch->calibration.cycles_count);
  printf("%d,", vswitch->calibration.min_value);
  printf("%d,", vswitch->calibration.max_value);
  printf("%d,", loop_duration_before_wait);
  printf("%d,", loop_duration);
  printf("%d,", vswitch->actuation.trigger_offset);
  printf("%d,", vswitch->actuation.rapid_trigger_offset);
  printf("%d,", vswitch->actuation.is_continuous_rapid_trigger_enabled);

  // Min/max calibration check
  printf("%d,", vswitch->state.value);
  printf("%d,", vswitch->calibration.min_value);
  printf("%d,", vswitch->calibration.max_value);

  // Motion check
  printf("%d,", vswitch->state.distance);
  printf("%d,", vswitch->state.velocity);
  printf("%d,", vswitch->state.is_pressing ? vswitch->state.motion.started_from : 0);
  printf("%d,", !vswitch->state.is_pressing ? vswitch->state.motion.started_from : 0);

  // Trigger/reset check
  printf("%d,", vswitch->state.distance);
  printf("%d,", vswitch->actuation.trigger_offset);
  if (vswitch->actuation.status == RESET) {
    printf("%d,", vswitch->actuation.rapid_trigger_offset > 0 ? vswitch->actuation.changed_at + vswitch->actuation.rapid_trigger_offset : 0);
  } else if (vswitch->actuation.status == TRIGGERED) {
    printf("%d,", vswitch->actuation.rapid_trigger_offset > 0 ? vswitch->actuation.changed_at - vswitch->actuation.rapid_trigger_offset : 0);
  }

  printf("0*/\n");
#endif
}

// TODO: constant loop time

// TODO: rapid trigger
// TODO: continuous rapid trigger

// TODO: better cycle calculation, maybe DMA ? or sleep by sample ? let's try to have better spacing between analysis

// TODO: Batch on_trigger
// TODO: browser configurator

// TODO: trigger point by switch