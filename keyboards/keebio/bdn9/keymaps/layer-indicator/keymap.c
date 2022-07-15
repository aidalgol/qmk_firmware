/* Copyright 2022 Aidan Gauland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "print.h"
#include "color.h"
#include "rgblight.h"
#include "encoder.h"

enum layers
{
    BASE,
    ARROW,
    PGUPDN,
    FKEY,
    // Not actually a layer.  Do not use, as it wil be unreachable.
    NUM_LAYERS
};
#define LAST_LAYER (NUM_LAYERS - 1)

enum encoder_names
{
    ENC_LEFT,
    ENC_RIGHT,
    ENC_MIDDLE
};

enum custom_keycodes
{
    CLR_LAYERS = SAFE_RANGE, // Reset to base layer.
    PREV_LAYER,
    NEXT_LAYER
};

void set_hs(uint8_t hue, uint8_t sat) {
    rgblight_sethsv_noeeprom(hue, sat, rgblight_get_val());
}

void keyboard_post_init_user(void) {
    debug_enable = true;
    debug_keyboard = true;
    debug_matrix = true;

    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(0, 0, rgblight_get_val());
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_MUTE, CLR_LAYERS, KC_MSEL,
        KC_MPRV,    KC_MPLY, KC_MNXT,
        KC_MRWD,    KC_MSTP, KC_MFFD
    ),

    // The mapping for top middle should always be transparent, so that the
    // "reset to base layer" button is always available.

    [ARROW] = LAYOUT(
        QK_BOOT, KC_TRNS,  KC_STOP,
         KC_DEL,   KC_UP,  KC_PENT,
        KC_LEFT, KC_DOWN, KC_RIGHT
    ),

    [PGUPDN] = LAYOUT(
        KC_PSCR, KC_TRNS, KC_PAUS,
         KC_INS, KC_HOME, KC_PGUP,
         KC_DEL,  KC_END, KC_PGDN
    ),

    [FKEY] = LAYOUT(
         KC_NO, KC_TRNS,  KC_NO,
        KC_F14,  KC_F15, KC_F16,
        KC_F17,  KC_F18, KC_APP
    ),
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        [ENC_MIDDLE] = ENCODER_CCW_CW(PREV_LAYER, NEXT_LAYER),
        [ENC_RIGHT]  = ENCODER_CCW_CW(RGB_VAD, RGB_VAI)
    },

    // The mappings for middle encoder must always be transparent, otherwise you
    // will get trapped on a layer.

    [ARROW] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_MIDDLE] = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_RIGHT]  = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
    },

    [PGUPDN] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_MIDDLE] = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_RIGHT]  = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
    },

    [FKEY] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_MIDDLE] = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_RIGHT]  = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
    },
};
// clang-format on

#define current_layer get_highest_layer(layer_state | default_layer_state)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case CLR_LAYERS:
#ifdef CONSOLE_ENABLE
                uprint("Resetting to base layer.\n");
#endif
                layer_clear();
                return false;
            case PREV_LAYER:
#ifdef CONSOLE_ENABLE
                uprint("Moving down a layer.\n");
#endif
                layer_move(MAX(current_layer - 1, BASE));
                return false;
            case NEXT_LAYER:
#ifdef CONSOLE_ENABLE
                uprint("Moving up a layer.\n");
#endif
                layer_move(MIN(current_layer + 1, LAST_LAYER));
                return false;
        }
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
#ifdef CONSOLE_ENABLE
    uprintf("At layer %d.\n", get_highest_layer(state));
#endif
    switch (get_highest_layer(state)) {
        case ARROW:
            set_hs(170, 255);
            break;

        case PGUPDN:
            set_hs(201, 255);
            break;

        case FKEY:
            set_hs(85, 255);
            break;

        default: // For any other layers, or the default layer
            set_hs(0, 0);
            break;
    }
    return state;
}
