#pragma once

enum macro_keycodes {
    SPC4 = SAFE_RANGE,
    _MACRO_SENTINEL
};
#define MACRO_KEYCODE_MIN SAFE_RANGE
#define MACRO_KEYCODE_MAX ( _MACRO_SENTINEL - 1 )
#define MACRO_KEYCODES    MACRO_KEYCODE_MIN ... MACRO_KEYCODE_MAX

bool process_macro(uint16_t keycode, keyrecord_t *record);
