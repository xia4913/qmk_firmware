#include QMK_KEYBOARD_H
#include "tap_dance.h"

static td_state_t td_state;

uint8_t cur_dance(qk_tap_dance_state_t *state) {
    switch (state->count) {
        case 1:
            if (state->interrupted || state->pressed) {
                return SINGLE_HOLD;
            }
            return SINGLE_TAP;
        default:
            return TD_STATE_NONE;
  }
}

void dance_safe_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 3) {
        reset_keyboard();
    }
    reset_tap_dance(state);
}

void dance_shift_enter_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case SINGLE_TAP:
            register_code16(KC_ENT);
            /* FALLTHRU */
        case SINGLE_HOLD:
            register_mods(MOD_BIT(KC_LSFT));
            return;
        default:
            return;
    }
}
void dance_shift_enter_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case SINGLE_TAP:
            unregister_code16(KC_ENT);
            /* FALLTHRU */
        case SINGLE_HOLD:
            unregister_mods(MOD_BIT(KC_LSFT));
            return;
        default:
            return;
    }
}

void dance_alt_grave_finished(qk_tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state);
    switch (td_state) {
        case SINGLE_TAP:
            SEND_STRING(SS_LALT("`"));
            reset_tap_dance(state);
            return;
        case SINGLE_HOLD:
            register_mods(MOD_BIT(KC_LALT));
            return;
        default:
            return;
    }
}
void dance_alt_grave_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case SINGLE_HOLD:
            unregister_mods(MOD_BIT(KC_LALT));
            return;
        default:
            return;
    }
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [SAFE_RESET]  = ACTION_TAP_DANCE_FN(dance_safe_reset),
    [SFT_ENT]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,
                                                 dance_shift_enter_finished,
                                                 dance_shift_enter_reset),
    [ALT_GRV]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,
                                                 dance_alt_grave_finished,
                                                 dance_alt_grave_reset),
};
