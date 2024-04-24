#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <hw/hid/kb/sets/set1.h>

#define KB_MOD_SHIFT 1 << 0
#define KB_MOD_ALT 1 << 1
#define KB_MOD_LEFT_SHIFT 1 << 2
#define KB_MOD_LEFT_ALT 1 << 3
#define KB_MOD_RIGHT_SHIFT 1 << 4
#define KB_MOD_RIGHT_ALT 1 << 5
#define KB_MOD_NUM 1 << 6
#define KB_MOD_CAPS 1 << 7
#define KB_MOD_CTRL 1 << 8
#define KB_MOD_LEFT_CTRL 1 << 9
#define KB_MOD_RIGHT_CTRL 1 << 10
#define KB_MOD_SCROLL 1 << 11

#define KB_POS(row, col) (uint8_t)((col & 0b00011111) | ((row & 0b111) << 5))

#define KB_KEY_ESC KB_POS(0, 0)
#define KB_KEY_F1 KB_POS(0, 1)
#define KB_KEY_F2 KB_POS(0, 2)
#define KB_KEY_F3 KB_POS(0, 3)
#define KB_KEY_F4 KB_POS(0, 4)
#define KB_KEY_F5 KB_POS(0, 5)
#define KB_KEY_F6 KB_POS(0, 6)
#define KB_KEY_F7 KB_POS(0, 7)
#define KB_KEY_F8 KB_POS(0, 8)
#define KB_KEY_F9 KB_POS(0, 9)
#define KB_KEY_F10 KB_POS(0, 10)
#define KB_KEY_F11 KB_POS(0, 11)
#define KB_KEY_F12 KB_POS(0, 12)
#define KB_KEY_INSERT KB_POS(0, 13)
#define KB_KEY_DELETE KB_POS(0, 14)

#define KB_KEY_BACK_TICK KB_POS(1, 0)
#define KB_KEY_1 KB_POS(1, 1)
#define KB_KEY_2 KB_POS(1, 2)
#define KB_KEY_3 KB_POS(1, 3)
#define KB_KEY_4 KB_POS(1, 4)
#define KB_KEY_5 KB_POS(1, 5)
#define KB_KEY_6 KB_POS(1, 6)
#define KB_KEY_7 KB_POS(1, 7)
#define KB_KEY_8 KB_POS(1, 8)
#define KB_KEY_9 KB_POS(1, 9)
#define KB_KEY_0 KB_POS(1, 10)
#define KB_KEY_MINUS KB_POS(1, 11)
#define KB_KEY_EQUALS KB_POS(1, 12)
#define KB_KEY_BACKSPACE KB_POS(1, 13)

#define KB_KEY_TAB KB_POS(2, 0)
#define KB_KEY_Q KB_POS(2, 1)
#define KB_KEY_W KB_POS(2, 2)
#define KB_KEY_E KB_POS(2, 3)
#define KB_KEY_R KB_POS(2, 4)
#define KB_KEY_T KB_POS(2, 5)
#define KB_KEY_Y KB_POS(2, 6)
#define KB_KEY_U KB_POS(2, 7)
#define KB_KEY_I KB_POS(2, 8)
#define KB_KEY_O KB_POS(2, 9)
#define KB_KEY_P KB_POS(2, 10)
#define KB_KEY_BRACKET_LEFT KB_POS(2, 11)
#define KB_KEY_BRACKET_RIGHT KB_POS(2, 12)
#define KB_KEY_BACKSLASH KB_POS(2, 13)

#define KB_KEY_CAPS_LOCK KB_POS(3, 0)
#define KB_KEY_A KB_POS(3, 1)
#define KB_KEY_S KB_POS(3, 2)
#define KB_KEY_D KB_POS(3, 3)
#define KB_KEY_F KB_POS(3, 4)
#define KB_KEY_G KB_POS(3, 5)
#define KB_KEY_H KB_POS(3, 6)
#define KB_KEY_J KB_POS(3, 7)
#define KB_KEY_K KB_POS(3, 8)
#define KB_KEY_L KB_POS(3, 9)
#define KB_KEY_SEMICOLON KB_POS(3, 10)
#define KB_KEY_SINGLE_QUOTE KB_POS(3, 11)
#define KB_KEY_ENTER KB_POS(3, 12)

#define KB_KEY_LSHIFT KB_POS(4, 0)
#define KB_KEY_Z KB_POS(4, 1)
#define KB_KEY_X KB_POS(4, 2)
#define KB_KEY_C KB_POS(4, 3)
#define KB_KEY_V KB_POS(4, 4)
#define KB_KEY_B KB_POS(4, 5)
#define KB_KEY_N KB_POS(4, 6)
#define KB_KEY_M KB_POS(4, 7)
#define KB_KEY_COMMA KB_POS(4, 8)
#define KB_KEY_PERIOD KB_POS(4, 9)
#define KB_KEY_SLASH KB_POS(4, 10)
#define KB_KEY_RSHIFT KB_POS(4, 11)

#define KB_KEY_LCTRL KB_POS(5, 0)
#define KB_KEY_FUNCTION KB_POS(5, 1) /* This will never be pressed */
#define KB_KEY_LOGO KB_POS(5, 2)
#define KB_KEY_ALT KB_POS(5, 3)
#define KB_KEY_SPACE KB_POS(5, 4)
#define KB_KEY_ALTGR KB_POS(5, 5)
#define KB_KEY_RCTRL KB_POS(5, 6)

#define KB_KEY_PAGE_UP KB_POS(6, 0)
#define KB_KEY_ARROW_UP KB_POS(6, 1)
#define KB_KEY_PAGE_DOWN KB_POS(6, 2)
#define KB_KEY_ARROW_LEFT KB_POS(6, 3)
#define KB_KEY_ARROW_DOWN KB_POS(6, 4)
#define KB_KEY_ARROW_RIGHT KB_POS(6, 5)

void kb_init();
bool kb_get_key(uint8_t keycode);
uint16_t kb_get_mods();
void kb_update();

void kb_internal_set_key(uint8_t keycode, bool pressed);
