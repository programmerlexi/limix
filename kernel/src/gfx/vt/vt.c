#include "kernel/gfx/vt/vt.h"
#include "kernel/config.h"
#include "kernel/gfx/drm.h"
#include "kernel/gfx/vt/ansi.h"
#include "kernel/kernel.h"
#include "kernel/mm/mm.h"
#include "libk/ipc/spinlock.h"
#include "libk/math/lib.h"
#include "libk/utils/memory/memory.h"
#include <stdbool.h>
#include <stddef.h>

static vt_char_t *_vt_buffer;
static u64 _vt_height;
static u64 _vt_width;
static u64 _vt_x;
static u64 _vt_y;

static u64 _dirty_start;
static u64 _dirty_end;
static bool _dirty;
static bool _full_redraw;

static u32 _vt_lock;

static ansi_state_t _state;

static u64 _attached_drm;
static bool _avail;

#define lock_vt spinlock(&_vt_lock)
#define unlock_vt spinunlock(&_vt_lock)

bool vt_is_available() { return _avail; }

void vt_init(u64 attached_to_drm) {
  _attached_drm = attached_to_drm;
  _vt_width = drm_width(_attached_drm) / 8;
  _vt_height = drm_height(_attached_drm) / 17;
  _vt_buffer = (vt_char_t *)request_page_block(
      ((_vt_width * _vt_height * sizeof(vt_char_t)) + 4095) / 4096);
  if (_vt_buffer == NULL) {
    kernel_panic_error("Couldn't initialize VT!");
  }
  _state.gr.font_state = 0;
  _state.gr.bg_index = 0;
  _state.gr.fg_index = 7;
  _avail = true;
  // vt_clear();
}
void vt_clear() {
  lock_vt;
  _vt_x = 0;
  _vt_y = 0;
  kmemset(_vt_buffer, 0, _vt_height * _vt_width * sizeof(vt_char_t));
  _full_redraw = true;
  unlock_vt;
  vt_flush();
}
void vt_draw_char(u64 i) {
  vt_char_t c = _vt_buffer[i];
  u64 cx = i % _vt_width;
  u64 cy = i / _vt_width;
  if (c.unicode)
    drm_plot_char_solid(_attached_drm, cx * 8, cy * 17, c.unicode,
                        c.fg.fb_color, c.bg.fb_color);
  else
    drm_fill_rel_rect(_attached_drm, cx * 8, cy * 17, 8, 17, c.bg.fb_color);
}
void vt_flush() {
  if (drm_is_attached_to_process(_attached_drm))
    return;
  if (_full_redraw) {
    drm_clear(_attached_drm);
    for (u64 i = 0; i < (_vt_width * _vt_height); i++) {
      vt_draw_char(i);
    }
    _full_redraw = false;
    _dirty = false;
  }
  if (_dirty) {
    // drm_fill_rel_rect(attached_drm, 0, vt_y * 17, vt_width * 8, 34,
    // 0x000000);
    for (u64 i = _dirty_start; i <= _dirty_end; i++) {
      if (i >= _vt_width * _vt_height)
        break;
      vt_draw_char(i);
    }
    _dirty = false;
  }
  _dirty_start = _vt_width * _vt_height;
  _dirty_end = 1;
  drm_sync();
}

void vt_redraw() {
  _full_redraw = true;
  vt_flush();
}

void vt_advance_y() {
  _vt_y++;
  _dirty = true;
  if (_vt_y >= _vt_height) {
    _vt_y -= CONFIG_SCROLL_STEP;
    kmemmove(_vt_buffer,
             (void *)((uintptr_t)_vt_buffer +
                      sizeof(vt_char_t) * _vt_width * CONFIG_SCROLL_STEP),
             _vt_width * (_vt_height - CONFIG_SCROLL_STEP) * sizeof(vt_char_t));
    kmemset((void *)((uintptr_t)_vt_buffer +
                     sizeof(vt_char_t) *
                         (_vt_width * (_vt_height - CONFIG_SCROLL_STEP))),
            0, _vt_width * sizeof(vt_char_t) * CONFIG_SCROLL_STEP);
    _full_redraw = true;
  }
  vt_flush();
}
void vt_advance_x() {
  _vt_x++;
  if (_vt_x >= _vt_width) {
    _vt_x = 0;
    vt_advance_y();
  }
}

bool termcode = false;

void kprint(char *s) {
  lock_vt;
  while (*s) {
    kprintc(*s);
    s++;
  }
  unlock_vt;
  vt_flush();
}
void kprintc(char c) {
  if (_state.working) {
    _state = ansi_process(_state, c);
  } else if (termcode) {
    switch (c) {
    case 0x01:
      _state.gr.font_state = 0;
      _state.gr.bg_index = 0;
      _state.gr.fg_index = 7;
      break;
    case 0x02:
      _state.gr.font_state |= VT_DIM;
      break;
    case 0x03:
      _state.gr.font_state |= VT_BOLD;
      break;
    case 0x04:
      _state.gr.font_state |= VT_REVERSE;
      break;
    case 0x05:
      _state.gr.font_state &= ~VT_DIM;
      break;
    case 0x06:
      _state.gr.font_state &= ~VT_BOLD;
      break;
    case 0x07:
      _state.gr.font_state &= ~VT_REVERSE;
      break;
    case 0x11:
      _state.gr.fg_index = 0;
      break;
    case 0x12:
      _state.gr.fg_index = 1;
      break;
    case 0x13:
      _state.gr.fg_index = 2;
      break;
    case 0x14:
      _state.gr.fg_index = 3;
      break;
    case 0x15:
      _state.gr.fg_index = 4;
      break;
    case 0x16:
      _state.gr.fg_index = 5;
      break;
    case 0x17:
      _state.gr.fg_index = 6;
      break;
    case 0x18:
      _state.gr.fg_index = 7;
      break;
    case 0x21:
      _state.gr.bg_index = 0;
      break;
    case 0x22:
      _state.gr.bg_index = 1;
      break;
    case 0x23:
      _state.gr.bg_index = 2;
      break;
    case 0x24:
      _state.gr.bg_index = 3;
      break;
    case 0x25:
      _state.gr.bg_index = 4;
      break;
    case 0x26:
      _state.gr.bg_index = 5;
      break;
    case 0x27:
      _state.gr.bg_index = 6;
      break;
    case 0x28:
      _state.gr.bg_index = 7;
      break;
    }
    termcode = false;
  } else {
    switch (c) {
    case LF:
      _dirty = true;
      _dirty_start = min(_vt_y * _vt_width + _vt_x, _dirty_start);
      _dirty_end = max(_vt_y * _vt_width + _vt_x, _dirty_end);
      vt_flush();
      vt_advance_y();
#ifdef LOG_SERIAL
      serial_write(c);
#endif
      break;
    case CR:
      _vt_x = 0;
#ifdef LOG_SERIAL
      serial_write(c);
#endif
      break;
    case BS:
      _vt_x--;
#ifdef LOG_SERIAL
      serial_write(c);
#endif
      break;
    case ESC:
      _state.working = true;
      _state.as = C1;
      break;
    case BEL:
      termcode = true;
      break;
    default:
      _dirty_start = min(_vt_y * _vt_width + _vt_x, _dirty_start);
      _dirty_end = max(_vt_y * _vt_width + _vt_x, _dirty_end);
      _vt_buffer[_vt_y * _vt_width + _vt_x].unicode = c;
      _vt_buffer[_vt_y * _vt_width + _vt_x].fg.fb_color =
          ansi_convert_fg(_state.gr);
      _vt_buffer[_vt_y * _vt_width + _vt_x].bg.fb_color =
          ansi_convert_bg(_state.gr);
      _dirty = true;
      _dirty_start = min(_vt_y * _vt_width + _vt_x, _dirty_start);
      _dirty_end = max(_vt_y * _vt_width + _vt_x, _dirty_end);
      vt_advance_x();
#ifdef LOG_SERIAL
      serial_write(c);
#endif
      break;
    }
  }
}
