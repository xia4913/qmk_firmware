#ifdef OLED_DRIVER_ENABLE
#    include QMK_KEYBOARD_H

extern uint8_t oled_buffer[];
extern uint8_t *oled_cursor;
extern uint8_t oled_rotation_width;
bool oled_is_cursor_at_bol(void) {
    return (oled_cursor - &oled_buffer[0]) % oled_rotation_width == 0;
}

extern const unsigned char font[] PROGMEM;
const unsigned char *glyph(unsigned char ascii) {
    const unsigned int font_offset = (ascii - OLED_FONT_START) * OLED_FONT_WIDTH;
    return &font[font_offset];
}	


typedef struct {
    uint16_t keycode;
    uint8_t  mods;
    keypos_t position;
    bool     is_updated;
} last_key_t;
last_key_t last_key = {
    .is_updated = false,
};

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_master) {
        return OLED_ROTATION_270;
    } else {
        return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
    }
}

#include "layer.h"
static const char layer_char(void) {
    if (layer_state_is(_ADJUST)) {
        return 0x12;
    } else if (layer_state_is(_RAISE)) {
        return 0x18;
    } else if (layer_state_is(_LOWER)) {
        return 0x19;
    } else {
        return ' ';
    }
}
void oled_render_mod_and_layer(void) {
    const uint8_t mods = get_mods();
    oled_write_char(mods & MOD_MASK_GUI   ? 'G' : ' ', false);
    oled_write_char(mods & MOD_MASK_CTRL  ? 'C' : ' ', false);
    oled_write_char(mods & MOD_MASK_SHIFT ? 'S' : ' ', false);
    oled_write_char(mods & MOD_MASK_ALT   ? 'A' : ' ', false);
    oled_write_char(layer_char(), false);
    if (!oled_is_cursor_at_bol()) {
        oled_advance_page(true);
    }
}

void oled_render_last_key_position(void) {
    if (!last_key.is_updated) {
        return;
    }

    char buf[4];
    snprintf(buf, sizeof(buf), "%01xx%01x", last_key.position.row, last_key.position.col);
    oled_write_ln(buf, false);
}

#define NN    { '\0', '\0' }
#define LU(x) { (x), (x) ^ 0x20 }
const unsigned char keycode_to_ascii_table[][2] PROGMEM = {
    NN, NN, NN, NN,
    LU('a'), LU('b'), LU('c'), LU('d'), LU('e'), LU('f'), LU('g'), LU('h'), LU('i'), LU('j'),
    LU('k'), LU('l'), LU('m'), LU('n'), LU('o'), LU('p'), LU('q'), LU('r'), LU('s'), LU('t'),
    LU('u'), LU('v'), LU('w'), LU('x'), LU('y'), LU('z'),
    { '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' }, { '5', '%' },
    { '6', '^' }, { '7', '&' }, { '8', '*' }, { '9', '(' }, { '0', ')' },
    NN, NN, NN, NN,
    { ' ', ' ' }, { '-', '_' }, { '=', '+' }, { '[', '{' }, { ']', '}' },
    { '\\', '|' }, { '#', '~' }, { ';', ':' }, { '\'', '"' }, { '`', '~' },
    { ',', '<' }, { '.', '>' }, { '/', '?' },
};
unsigned char keycode_to_ascii(uint16_t keycode) {
    const int table_size = sizeof(keycode_to_ascii_table) / sizeof(keycode_to_ascii_table[0]);
    if (keycode < table_size) {
        return pgm_read_byte_near(&keycode_to_ascii_table[keycode][get_mods() & MOD_MASK_SHIFT ? 1 : 0]);
    }
    return '\0';
}
void oled_render_last_char(void) {
    uint8_t char_width = MIN(OLED_FONT_WIDTH, oled_max_chars());
    unsigned char buf[char_width];
    const unsigned char c = keycode_to_ascii(last_key.keycode);
    memcpy_P(buf, glyph(c), sizeof(buf));
    for (uint8_t y = 0; y < CHAR_BIT; y++) {
        for (uint8_t x = 0; x < char_width; x++) {
            oled_write_char(bit_is_set(buf[x], y) ? c : ' ', false);
        }
        if (!oled_is_cursor_at_bol()) {
            oled_advance_page(true);
	}
    }
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0
    };
    oled_write_P(crkbd_logo, false);
}

void oled_process_record_user(uint16_t keycode, keyrecord_t *record) {
    last_key.keycode    = keycode;
    last_key.position   = record->event.key;
    last_key.is_updated = true;
}

void oled_task_user(void) {
    if (is_master) {
        oled_render_mod_and_layer();
        if (!last_key.is_updated) {
            return;
        }
        oled_render_last_key_position();
        oled_advance_page(true);
        oled_render_last_char();
        last_key.is_updated = false;
    } else {
        oled_render_logo();
    }
}
#endif  // OLED_DRIVER_ENABLE
