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
    SECOND,
    // Not actually a layer.  Do not use, as it wil be unreachable.
    NUM_LAYERS
};
#define LAST_LAYER (NUM_LAYERS - 1)

enum encoder_names
{
    ENC_LEFT,
    ENC_RIGHT,
    ENC_MIDDLE,
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
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_WHITE);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_MUTE, CLR_LAYERS, KC_MPLY,
         KC_F19,     KC_F20,  KC_F21,
         KC_F22,     KC_F23,  KC_F24
    ),

    [SECOND] = LAYOUT(
        QK_BOOT, KC_TRNS, KC_STOP,
        KC_F13,  KC_F14,  KC_F15,
        KC_F16,  KC_F17,  KC_F18
    ),
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        [ENC_MIDDLE] = ENCODER_CCW_CW(PREV_LAYER, NEXT_LAYER),
        [ENC_RIGHT]  = ENCODER_CCW_CW(KC_MEDIA_PREV_TRACK, KC_MEDIA_NEXT_TRACK)
    },
    [SECOND] = {
        [ENC_LEFT]   = ENCODER_CCW_CW(KC_NO, KC_NO),
        [ENC_MIDDLE] = ENCODER_CCW_CW(KC_TRNS, KC_TRNS),
        [ENC_RIGHT]  = ENCODER_CCW_CW(RGB_VAD, RGB_VAI)
    }
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
    uprintf("At layer %d.\n", state);
#endif
    switch (get_highest_layer(state)) {
        case SECOND:
            set_hs(170, 255);
            break;
        default: // For any other layers, or the default layer
            set_hs(0, 0);
            break;
    }
    return state;
}
