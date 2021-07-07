/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "layer.h"
#include "tap_dance.h"

#ifdef OLED_DRIVER_ENABLE
#    include "oled.h"
#endif  // OLED_DRIVER_ENABLE

#define LOWER    MO(_LOWER)
#define RAISE    MO(_RAISE)
#define ADJUST   MO(_ADJUST)

#define RESET    TD(TD_SAFE_RESET)  // override

#define CTL_SPC  TD(TD_CTL_OR_SPC)
#define SFT_ENT  TD(TD_SFT_OR_ENT)
#define GUI_MHEN GUI_T(KC_MHEN)

#define CTLxSPC  TD(TD_CTL_AND_SPC)
#define SFTxENT  TD(TD_SFT_AND_ENT)
#define ALTxIME  TD(TD_ALT_AND_GRV)

#define SPC4     TD(TD_SPC4)
#define GRV_HOME TD(TD_GRV_HOME)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        KC_LBRC, KC_RBRC, KC_COMM,  KC_DOT,    KC_Z,    KC_J,                         KC_P,    KC_C,    KC_K,    KC_Y,    KC_L,    KC_G,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
         KC_EQL,    KC_O,    KC_A,    KC_I,    KC_E,    KC_U,                         KC_N,    KC_S,    KC_T,    KC_H,    KC_R, KC_MINS,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        KC_BSLS, KC_SLSH, KC_SCLN, KC_QUOT,    KC_V,    KC_Q,                         KC_B,    KC_F,    KC_W,    KC_D,    KC_M,    KC_X,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           GUI_MHEN,   LOWER, CTL_SPC,    SFT_ENT,   RAISE, ALTxIME
                                        //`--------------------------'  `--------------------------'
    ),
    [_LOWER] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        _______, _______, _______, _______, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, _______,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        _______, _______, _______, _______,GRV_HOME, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            _______, _______, _______,    _______,  ADJUST, _______
                                        //`--------------------------'  `--------------------------'
    ),
    [_RAISE] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
         KC_ESC, XXXXXXX, KC_PGUP, KC_PGDN,  XXXXXXX, XXXXXXX,                       KC_UP, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, KC_HOME,  KC_TAB,  KC_END, XXXXXXX,                      KC_DOWN, XXXXXXX, XXXXXXX, KC_BSPC, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT,KC_RIGHT, XXXXXXX,  KC_DEL, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            _______,  ADJUST,    SPC4,    _______, _______, _______
                                        //`--------------------------'  `--------------------------'
    ),
    [_ADJUST] = LAYOUT_split_3x6_3(
    //,-----------------------------------------------------.                    ,-----------------------------------------------------.
          RESET, XXXXXXX, XXXXXXX, XXXXXXX, DM_PLY1, DM_REC1,                      DM_REC2, DM_PLY2, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                        KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        XXXXXXX, RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX,                      XXXXXXX, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX,
    //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                            _______, _______, CTLxSPC,    SFTxENT, _______, _______
                                        //`--------------------------'  `--------------------------'
    )
};

#ifdef OLED_DRIVER_ENABLE
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_process_record_user(keycode, record);
    }
    return true;
}
#endif  // OLED_DRIVER_ENABLE
