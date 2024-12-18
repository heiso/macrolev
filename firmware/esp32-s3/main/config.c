#include "config.h"
#include "DRV2605L.h"
#include "keyboard.h"
#include "stdlib.h"
#include <class/hid/hid.h>

const struct user_config keyboard_default_user_config = {
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
    {{0, 1}, {0, 0}, {0, 13}, {0, 12}, {1, 0}, {1, 12}, {2, 1}, {2, 0}, {2, 12}, {3, 1}, {3, 0}, {3, 12}, {4, 1}, {4, 0}, {4, 13}},
    {{0, 2}, {0, 7}, {0, 10}, {1, 2}, {1, 7}, {1, 10}, {2, 3}, {2, 7}, {2, 10}, {3, 3}, {3, 7}, {3, 11}, {4, 4}, {4, 8}, {4, 12}},
    {{0, 3}, {0, 6}, {0, 11}, {1, 3}, {1, 6}, {1, 11}, {2, 4}, {2, 6}, {2, 11}, {3, 4}, {3, 8}, {4, 2}, {4, 5}, {4, 7}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 5}, {0, 8}, {1, 1}, {1, 5}, {1, 8}, {2, 2}, {2, 5}, {2, 8}, {3, 2}, {3, 6}, {3, 9}, {4, 3}, {4, 10}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 4}, {0, 9}, {1, 4}, {XXXX, XXXX}, {1, 9}, {XXXX, XXXX}, {2, 9}, {XXXX, XXXX}, {3, 5}, {3, 10}, {XXXX, XXXX}, {4, 6}, {4, 9}, {4, 11}},
};