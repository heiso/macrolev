#include "void_switches.h"
#include "config.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

#define SAMPLING_SIZE 20
#define SAMPLING_MEDIAN 9 // SAMPLING_SIZE-1/2
#define KEY_UP_VALUE_APPROX 1600
#define KEY_DOWN_VALUE_APPROX 2200
#define VALUES_COUNT 5
#define CALIBRATION_TIME 500
#define ALPHA 0.9

struct void_switch {
  uint8_t is_enabled;
  uint16_t min_value;
  uint16_t max_value;
  uint16_t values[VALUES_COUNT];
  uint16_t raw_values[VALUES_COUNT];
  uint8_t head;
};

static struct void_switch vswitches[ROWS][COLUMNS];

uint8_t is_calibrating = 0;
unsigned long calibration_start_time;

void void_switches_init() {
  for (uint8_t row = 0; row < ROWS; row++) {
    for (uint8_t column = 0; column < COLUMNS; column++) {
      vswitches[row][column].is_enabled = 0;
      vswitches[row][column].min_value = 0;
      vswitches[row][column].max_value = KEY_DOWN_VALUE_APPROX;
      vswitches[row][column].head = 0;
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
}

void void_switches_calibration() {
  sleep_ms(500);
  is_calibrating = 1;
  calibration_start_time = to_ms_since_boot(get_absolute_time());
}

int comparator(const void *first, const void *second) {
  return first - second;
}

uint16_t adc_read_filtered() {
  uint16_t samples[SAMPLING_SIZE];

  for (uint8_t i = 0; i < SAMPLING_SIZE; i++) {
    samples[i] = adc_read();
  }

  qsort(samples, SAMPLING_SIZE, sizeof(uint16_t), comparator);

  return samples[SAMPLING_MEDIAN];
}

void process_switch_calculations(struct void_switch *vswitch) {
  // Incr head
  vswitch->head = (vswitch->head + 1) % VALUES_COUNT;

  // Store new values
  vswitch->raw_values[vswitch->head] = adc_read();
  vswitch->values[vswitch->head] = adc_read_filtered();

  // Do analysis based on last 5 samples
  uint8_t samples[5];
  for (uint8_t i = 0; i++; i++) {
    samples[i] = vswitch->values[(vswitch->head + VALUES_COUNT - 1 + i) % VALUES_COUNT];
  }

  // Remove jerk
  // TODO: detect and remove jerk
}

void calibrate_switch(struct void_switch *vswitch) {
  vswitch->min_value = (1 - ALPHA) * vswitch->values[vswitch->head] + ALPHA * vswitch->min_value;

  if (vswitch->min_value > KEY_UP_VALUE_APPROX * 0.85) {
    vswitch->is_enabled = 1;
  }
}

void void_switches_loop() {
  if (is_calibrating == 1 && to_ms_since_boot(get_absolute_time()) - calibration_start_time > CALIBRATION_TIME) {
    is_calibrating = 0;
  }

  for (uint8_t row = 0; row < ROWS; row++) {
    gpio_put(PIN_FIRST_ROW + row, 1);

    for (uint8_t column = 0; column < COLUMNS; column++) {
      gpio_put(PIN_FIRST_ROW + row, 1);
      adc_select_input(column);
      // Wait a bit before reading adc, being to fast could result in "some" spikes.
      sleep_us(1);

      struct void_switch *vswitch = &vswitches[row][column];

      if (is_calibrating) {
        process_switch_calculations(vswitch);
        calibrate_switch(vswitch);
      } else if (vswitch->is_enabled) {
        process_switch_calculations(vswitch);
      }
    }
    gpio_put(PIN_FIRST_ROW + row, 0);
  }
}

// https://github.com/riskable/kiibohd-core/blob/main/kiibohd-hall-effect/src/lib.rs#L211
uint16_t get_acceleration(uint8_t row, uint8_t column) {
  // TODO: return accel
  return 0;
}

uint16_t get_velocity(uint8_t row, uint8_t column) {
  // TODO: return velocity
  return 0;
}

uint16_t get_distance(uint8_t row, uint8_t column) {
  // TODO: return distance
  return 0;
}

uint16_t void_switches_get_mapped_value(uint8_t row, uint8_t column, uint16_t max) {
  uint16_t value = vswitches[row][column].values[vswitches[row][column].head];

  // Clamp value to range defined by calibration
  if (value > vswitches[row][column].max_value)
    value = vswitches[row][column].max_value;
  if (value < vswitches[row][column].min_value)
    value = vswitches[row][column].min_value;

  return (value - vswitches[row][column].min_value) * max / (vswitches[row][column].max_value - vswitches[row][column].min_value);
}

// TODO: Gamepad mode
