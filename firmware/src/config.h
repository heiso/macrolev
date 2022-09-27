#ifndef CONFIG_H_
#define CONFIG_H_

#include "pico/stdlib.h"
#include "tusb.h"

// KB2040
#define I2C_INSTANCE i2c0
#define PIN_ENC_DT 2
#define PIN_ENC_CLK 3
#define PIN_ENC_SW 4
#define PIN_FIRST_ADC 26
#define PIN_I2C_SDA 12
#define PIN_I2C_SCL 13

// QT py 2040
// #define I2C_INSTANCE i2c1
// #define PIN_ENC_DT 3
// #define PIN_ENC_CLK 4
// #define PIN_ENC_SW 6
// #define PIN_HALL_EFFECT_SENSOR 26
// #define ADC_HALL_EFFECT_SENSOR 0
// #define PIN_I2C_SDA 22
// #define PIN_I2C_SCL 23

// Pico
// #define I2C_INSTANCE i2c0
// #define PIN_ENC_DT 6
// #define PIN_ENC_CLK 7
// #define PIN_ENC_SW 8
// #define PIN_HALL_EFFECT_SENSOR_1 26
// #define PIN_HALL_EFFECT_SENSOR_2 27
// #define ADC_HALL_EFFECT_SENSOR 0
// #define PIN_I2C_SDA ??
// #define PIN_I2C_SCL ??

#define KEY_COLUMN_COUNT 3 // One adc per column
#define KEY_ROW_COUNT 2

static uint16_t KEYMAP[KEY_COLUMN_COUNT * KEY_ROW_COUNT] = {
    HID_KEY_0, HID_KEY_1, HID_KEY_3,
    HID_KEY_VOLUME_DOWN, HID_KEY_VOLUME_UP, HID_KEY_6};

#endif