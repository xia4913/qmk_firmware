#include QMK_KEYBOARD_H
#include "macro.h"

bool macro_quad_space(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        tap_code(KC_SPC);
        tap_code(KC_SPC);
        tap_code(KC_SPC);
        tap_code(KC_SPC);
    }
    return true;
}

#define MACRO_INDEX(x) ( (x) - MACRO_KEYCODE_MIN )
typedef bool (*macro_fn_t)(uint16_t keycode, keyrecord_t *record);
macro_fn_t macro_functions[] = {
    [MACRO_INDEX(SPC4)] = macro_quad_space,
};

bool process_macro(uint16_t keycode, keyrecord_t *record) {
    int id = MACRO_INDEX(keycode);
    if (id >= sizeof(macro_functions)) {
        return false;
    }
    return macro_functions[id](keycode, record);
}
