#define DEBUG_MODULE "kb"

#include <config.h>
#include <debug.h>
#include <hw/hid/kb/kb.h>
#include <hw/hid/kb/poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <utils/memory/memory.h>

static bool key_state[256];
static bool caps_lock;
static bool num_lock;
static bool scroll_lock;

void kb_init() {
  log(LOGLEVEL_ANALYZE, "Zeroing keyboard state");
  memset(key_state, 0, 256);
}
bool kb_get_key(uint8_t k) { return key_state[k]; }
uint16_t kb_get_mods() {
  uint16_t s = 0;
  if (caps_lock)
    s |= KB_MOD_CAPS;
  if (num_lock)
    s |= KB_MOD_NUM;
  if (scroll_lock)
    s |= KB_MOD_SCROLL;
  if (kb_get_key(KB_KEY_LSHIFT))
    s |= KB_MOD_SHIFT | KB_MOD_LEFT_SHIFT;
  if (kb_get_key(KB_KEY_RSHIFT))
    s |= KB_MOD_SHIFT | KB_MOD_RIGHT_SHIFT;
  if (kb_get_key(KB_KEY_ALT))
    s |= KB_MOD_ALT | KB_MOD_RIGHT_ALT;
  if (kb_get_key(KB_KEY_ALTGR))
    s |= KB_MOD_ALT | KB_MOD_LEFT_ALT;
  if (kb_get_key(KB_KEY_LCTRL))
    s |= KB_MOD_CTRL | KB_MOD_LEFT_CTRL;
  if (kb_get_key(KB_KEY_RCTRL))
    s |= KB_MOD_CTRL | KB_MOD_RIGHT_CTRL;
  return s;
}

void kb_internal_set_key(uint8_t key, bool state) { key_state[key] = state; }
