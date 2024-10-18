#include <kernel/debug.h>
#include <kernel/hw/hid/kb/kb.h>
#include <libk/utils/memory/memory.h>
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "kb"

static bool _key_state[256];
static bool _caps_lock;
static bool _num_lock;
static bool _scroll_lock;

void kb_init() {
  log(LOGLEVEL_ANALYZE, "Zeroing keyboard state");
  kmemset(_key_state, 0, 256);
}
bool kb_get_key(u8 k) { return _key_state[k]; }
u16 kb_get_mods() {
  u16 s = 0;
  if (_caps_lock)
    s |= KB_MOD_CAPS;
  if (_num_lock)
    s |= KB_MOD_NUM;
  if (_scroll_lock)
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

void kb_internal_set_key(u8 key, bool state) { _key_state[key] = state; }
