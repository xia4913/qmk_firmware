#include QMK_KEYBOARD_H
#include "layer.h"
#include "tap_dance.h"

void dance_safe_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 3) {
        reset_keyboard();
    }
    reset_tap_dance(state);
}

typedef struct {
    const char *str;
    uint16_t   kc;
    uint8_t    count;
} dance_count_send_string_t;

void dance_count_send_string_on_each_tap(qk_tap_dance_state_t *state, void *user_data) {
    dance_count_send_string_t *data = (dance_count_send_string_t *)user_data;
    if (state->count == data->count) {
        send_string(data->str);
        state->finished = true;
    }
}
void dance_count_send_string_finished(qk_tap_dance_state_t *state, void *user_data) {
    dance_count_send_string_t *data = (dance_count_send_string_t *)user_data;
    if (state->count == data->count) {
        send_string(data->str);
    } else {
        register_code(data->kc);
    }
}
void dance_count_send_string_reset(qk_tap_dance_state_t *state, void *user_data) {
    dance_count_send_string_t *data = (dance_count_send_string_t *)user_data;
    if (state->count != data->count) {
        unregister_code(data->kc);
    }
}
#define ACTION_TAP_DANCE_COUNT_STRING(kc, str, count) \
    { \
        .fn = { \
            dance_count_send_string_on_each_tap, \
            dance_count_send_string_finished, \
            dance_count_send_string_reset \
        }, \
        .user_data = (void *)&((dance_count_send_string_t){ (str), (kc), (count) }), \
    }

typedef struct {
    uint16_t kc;
    uint8_t  mods;
} dance_mods_and_kc_t;

const int dance_mods_or_key_hold_count = 2;
void dance_mods_or_key_on_each_tap(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= dance_mods_or_key_hold_count + 1) {
        dance_mods_and_kc_t *data = (dance_mods_and_kc_t *)user_data;
        register_code(data->kc);
        if (state->count == dance_mods_or_key_hold_count + 1) {
            for (int i = dance_mods_or_key_hold_count; i > 0; i--) {
                register_code(data->kc);
            }
        }
    }
}
void dance_mods_or_key_finished(qk_tap_dance_state_t *state, void *user_data) {
    dance_mods_and_kc_t *data = (dance_mods_and_kc_t *)user_data;
    if (state->pressed) {
        switch (state->count) {
        case 2:
            tap_code(data->kc);
            /* FALLTHRU */
        case 1:
            register_mods(MOD_BIT(data->mods));
            return;
        }
    }
    if (state->count <= dance_mods_or_key_hold_count) {
        for (int i = state->count; i > 0; i--) {
            register_code(data->kc);
        }
    }
}
void dance_mods_or_key_reset(qk_tap_dance_state_t *state, void *user_data) {
    dance_mods_and_kc_t *data = (dance_mods_and_kc_t *)user_data;
    unregister_code(data->kc);
    unregister_mods(MOD_BIT(data->mods));
}
#define ACTION_TAP_DANCE_KEY_OR_MODS(kc, mods) \
    { \
        .fn = { \
            dance_mods_or_key_on_each_tap, \
            dance_mods_or_key_finished, \
            dance_mods_or_key_reset \
        }, \
        .user_data = (void *)&((dance_mods_and_kc_t){ (kc), (mods) }), \
    }

void dance_mods_and_key_finished(qk_tap_dance_state_t *state, void *user_data) {
    dance_mods_and_kc_t *data = (dance_mods_and_kc_t *)user_data;
    register_mods(MOD_BIT(data->mods));
    if (!(state->count == 1 && state->pressed)) {
        for (int i = state->count; i > 0; i--) {
            register_code(data->kc);
        }
    }
}
void dance_mods_and_key_reset(qk_tap_dance_state_t *state, void *user_data) {
    dance_mods_and_kc_t *data = (dance_mods_and_kc_t *)user_data;
    unregister_code(data->kc);
    unregister_mods(MOD_BIT(data->mods));
}
#define ACTION_TAP_DANCE_KEY_AND_MODS(kc, mods) \
    { \
        .fn = { \
            NULL, \
            dance_mods_and_key_finished, \
            dance_mods_and_key_reset \
        }, \
        .user_data = (void *)&((dance_mods_and_kc_t){ (kc), (mods) }), \
    }

typedef struct {
    uint16_t kc;
    uint8_t  mods;
    uint8_t  multiple;
} dance_multiple_t;

void dance_multiple_register_code(dance_multiple_t *data) {
    for (int i = data->multiple; i > 0; i--) {
        register_code(data->kc);
    }
}
void dance_multiple_on_each_tap(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count > 1) {
        dance_multiple_t *data = (dance_multiple_t *)user_data;
        dance_multiple_register_code(data);
    }
}
void dance_multiple_finished(qk_tap_dance_state_t *state, void *user_data) {
    dance_multiple_t *data = (dance_multiple_t *)user_data;
    if (state->pressed) {
        register_mods(MOD_BIT(data->mods));
        unregister_code(data->kc);
    } else if (!state->interrupted) {
        dance_multiple_register_code(data);
    }
}
void dance_multiple_reset(qk_tap_dance_state_t *state, void *user_data) {
    dance_multiple_t *data = (dance_multiple_t *)user_data;
    unregister_code(data->kc);
    unregister_mods(MOD_BIT(data->mods));
}
#define ACTION_TAP_DANCE_MULTIPLE(kc, mods, multiple) \
    { \
        .fn = { \
            dance_multiple_on_each_tap, \
            dance_multiple_finished, \
            dance_multiple_reset \
        }, \
        .user_data = (void *)&((dance_multiple_t){ (kc), (mods), (multiple) }), \
    }

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_SAFE_RESET]  = ACTION_TAP_DANCE_FN(dance_safe_reset),
    [TD_GRV_HOME]    = ACTION_TAP_DANCE_COUNT_STRING(KC_GRV, "~/",    2),
    [TD_SPC4]        = ACTION_TAP_DANCE_MULTIPLE(    KC_SPC, KC_LCTL, 4),
    [TD_ALT_AND_GRV] = ACTION_TAP_DANCE_KEY_AND_MODS(KC_GRV, KC_LALT),
    [TD_CTL_OR_SPC]  = ACTION_TAP_DANCE_KEY_OR_MODS( KC_SPC, KC_LCTL),
    [TD_CTL_AND_SPC] = ACTION_TAP_DANCE_KEY_AND_MODS(KC_SPC, KC_LCTL),
    [TD_SFT_OR_ENT]  = ACTION_TAP_DANCE_KEY_OR_MODS( KC_ENT, KC_LSFT),
    [TD_SFT_AND_ENT] = ACTION_TAP_DANCE_KEY_AND_MODS(KC_ENT, KC_LSFT),
};
