#include "layout.h"

// {adc_channel, amux_channel}
const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
    {{0, 10}, {0, 8}, {0, 7}, {0, 5}, {1, 9}, {1, 8}, {1, 6}, {2, 10}, {2, 9}, {2, 7}, {2, 6}, {3, 9}, {3, 8}, {3, 6}, {4, 2}},
    {{0, 11}, {0, 9}, {0, 6}, {0, 4}, {1, 10}, {1, 7}, {1, 5}, {2, 11}, {2, 8}, {2, 5}, {2, 4}, {3, 10}, {3, 7}, {3, 5}, {4, 1}},
    {{0, 12}, {0, 14}, {0, 2}, {1, 11}, {1, 14}, {1, 1}, {1, 4}, {2, 12}, {2, 15}, {2, 3}, {3, 11}, {3, 14}, {3, 1}, {3, 4}, {XXXX, XXXX}},
    {{XXXX, XXXX}, {0, 13}, {0, 0}, {0, 3}, {1, 13}, {1, 15}, {1, 2}, {1, 3}, {2, 14}, {2, 0}, {2, 2}, {3, 12}, {3, 15}, {XXXX, XXXX}, {3, 3}},
    {{XXXX, XXXX}, {0, 15}, {0, 1}, {1, 12}, {XXXX, XXXX}, {1, 0}, {XXXX, XXXX}, {2, 13}, {XXXX, XXXX}, {2, 1}, {3, 13}, {XXXX, XXXX}, {3, 0}, {3, 2}, {4, 0}},
};

// static uint8_t row_col_by_channels[ADC_CHANNEL_COUNT][AMUX_CHANNEL_COUNT][2] = {
//     {{3, 2}, {4, 2}, {2, 2}, {3, 3}, {1, 3}, {0, 3}, {1, 2}, {0, 2}, {0, 1}, {1, 1}, {0, 0}, {1, 0}, {2, 0}, {3, 1}, {2, 1}, {4, 1}},
//     {{4, 5}, {2, 5}, {3, 6}, {3, 7}, {2, 6}, {1, 6}, {0, 6}, {1, 5}, {0, 5}, {0, 4}, {1, 4}, {2, 3}, {4, 3}, {3, 4}, {2, 4}, {3, 5}},
//     {{3, 9}, {4, 9}, {3, 10}, {2, 9}, {1, 10}, {1, 9}, {0, 10}, {0, 9}, {1, 8}, {0, 8}, {0, 7}, {1, 7}, {2, 7}, {4, 7}, {3, 8}, {2, 8}},
//     {{4, 12}, {2, 12}, {4, 13}, {3, 13}, {2, 13}, {1, 13}, {0, 13}, {1, 12}, {0, 12}, {0, 11}, {1, 11}, {2, 10}, {3, 11}, {4, 10}, {2, 11}, {3, 12}},
//     {{4, 14}, {1, 14}, {0, 14}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}, {99, 99}},
// };

// clang-format off
const uint16_t keymaps[2][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE_LAYER] = {
        {HID_KEY_ESCAPE, HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE},
        {____, HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_ENTER},
        {____, HID_KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, HID_KEY_EUROPE_1, XXXX},
        {XXXX, HID_KEY_SHIFT_LEFT, HID_KEY_F3, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, XXXX},
        {XXXX, HID_KEY_CONTROL_LEFT, HID_KEY_ALT_LEFT, HID_KEY_GUI_LEFT, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_SPACE, XXXX, HID_KEY_GUI_RIGHT, HID_KEY_ALT_RIGHT, XXXX, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT},
    },
    [_TAP_LAYER] = {
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____},
        {____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, XXXX},
        {XXXX, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, HID_KEY_ARROW_UP, XXXX},
        {XXXX, ____, ____, ____, XXXX, ____, XXXX, ____, XXXX, ____, ____, XXXX, ____, ____, ____},
    },
};
// clang-format on