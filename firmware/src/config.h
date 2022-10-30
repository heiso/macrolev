#ifndef CONFIG_H_
#define CONFIG_H_

#include "pico/stdlib.h"
#include "tusb.h"

// KB2040
#define I2C_INSTANCE i2c0
#define PIN_ENC_DT 2
#define PIN_ENC_CLK 3
#define PIN_ENC_SW 4
#define PIN_FIRST_COLUMN 26 // Must be an ADC pin
#define PIN_FIRST_ROW 2
#define PIN_I2C_SDA 12
#define PIN_I2C_SCL 13

// QT py 2040
// #define I2C_INSTANCE i2c1
// #define PIN_ENC_DT 3
// #define PIN_ENC_CLK 4
// #define PIN_ENC_SW 6
// #define PIN_FIRST_COLUMN 26
// #define PIN_FIRST_ROW 24
// #define PIN_I2C_SDA 22
// #define PIN_I2C_SCL 23

// Pico
// #define I2C_INSTANCE i2c1
// #define PIN_ENC_DT 6
// #define PIN_ENC_CLK 7
// #define PIN_ENC_SW 8
// #define PIN_FIRST_COLUMN 26
// #define PIN_FIRST_ROW 21
// #define PIN_I2C_SDA 18
// #define PIN_I2C_SCL 19

#define COLUMNS 3 // One adc per column
#define ROWS 1

// static uint8_t KEYMAP_CONSUMER[ROWS][COLUMNS] = {
//     {HID_USAGE_CONSUMER_VOLUME_DECREMENT, HID_USAGE_CONSUMER_PLAY_PAUSE, HID_USAGE_CONSUMER_VOLUME_INCREMENT},
//     {HID_USAGE_CONSUMER_MUTE, HID_KEY_NONE, HID_KEY_NONE}};

// static uint8_t KEYMAP[ROWS][COLUMNS] = {
//     {HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE},
//     {HID_KEY_NONE, HID_KEY_SEMICOLON, HID_KEY_D}};

// static uint8_t KEYMAP_MODIFIERS[ROWS][COLUMNS] = {
//     {HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE},
//     {HID_KEY_NONE, KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT, KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT}};

#endif