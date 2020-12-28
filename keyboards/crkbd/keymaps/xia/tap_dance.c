#include QMK_KEYBOARD_H
#include "tap_dance.h"

void dance_safe_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 3) {
        reset_keyboard();
    }
    reset_tap_dance(state);
}

typedef struct {
    uint16_t kc;
    uint8_t  mods;
} qk_tap_dance_mods_t;
void qk_tap_dance_mods_finished(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_dance_mods_t *data = (qk_tap_dance_mods_t *)user_data;
    register_mods(MOD_BIT(data->mods));
    if (!(state->count == 1 && state->pressed)) {
        for (int i = state->count; i > 0; i--) {
            register_code16(data->kc);
        }
    }
}
void qk_tap_dance_mods_reset(qk_tap_dance_state_t *state, void *user_data) {
    qk_tap_dance_mods_t *data = (qk_tap_dance_mods_t *)user_data;
    if (!(state->count == 1 && state->pressed)) {
        unregister_code16(data->kc);
    }
    unregister_mods(MOD_BIT(data->mods));
}
#define ACTION_TAP_DANCE_MODS(kc, mods) \
    { \
        .fn = { NULL, qk_tap_dance_mods_finished, qk_tap_dance_mods_reset }, \
        .user_data = (void *)&((qk_tap_dance_mods_t){ (kc), (mods) }), \
    }

qk_tap_dance_action_t tap_dance_actions[] = {
    [SAFE_RESET] = ACTION_TAP_DANCE_FN(dance_safe_reset),
    [CTL_SPC]    = ACTION_TAP_DANCE_MODS(KC_SPC, KC_LCTL),
    [SFT_ENT]    = ACTION_TAP_DANCE_MODS(KC_ENT, KC_LSFT),
    [ALT_GRV]    = ACTION_TAP_DANCE_MODS(KC_GRV, KC_LALT),
};
