#include "config.h"
#include <gfx/drm.h>
#include <gfx/vt/ansi.h>
#include <gfx/vt/vt.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <kipc/spinlock.h>
#include <math/lib.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/memory/memory.h>

vt_char_t *vt_buffer;
uint64_t vt_height;
uint64_t vt_width;
uint64_t vt_x;
uint64_t vt_y;

uint64_t dirty_start;
uint64_t dirty_end;
bool dirty;
bool full_redraw;

uint32_t vt_lock;

ansi_state_t state;

uint64_t attached_drm;

#define lock_vt spinlock(&vt_lock)
#define unlock_vt spinunlock(&vt_lock)

void vt_init(uint64_t attached_to_drm) {
  attached_drm = attached_to_drm;
  vt_width = drm_width(attached_drm) / 8;
  vt_height = drm_height(attached_drm) / 17;
  vt_buffer = (vt_char_t *)request_page_block(
      ((vt_width * vt_height * sizeof(vt_char_t)) + 4095) / 4096);
  if (vt_buffer == NULL) {
    serial_writes("[!!] Couldn't initialize VT!\n\r");
    hcf();
  }
  state.gr.font_state = 0;
  state.gr.bg_index = 0;
  state.gr.fg_index = 7;
  // vt_clear();
}
void vt_clear() {
  lock_vt;
  vt_x = 0;
  vt_y = 0;
  memset(vt_buffer, 0, vt_height * vt_width * sizeof(vt_char_t));
  full_redraw = true;
  unlock_vt;
  vt_flush();
}
void vt_draw_char(uint64_t i) {
  vt_char_t c = vt_buffer[i];
  uint64_t cx = i % vt_width;
  uint64_t cy = i / vt_width;
  drm_fill_rel_rect(attached_drm, cx * 8, cy * 17, 8, 17, c.bg.fb_color);
  if (c.unicode)
    drm_plot_char(attached_drm, cx * 8, cy * 17, c.unicode, c.fg.fb_color);
}
void vt_flush() {
  if (drm_is_attached_to_process(attached_drm))
    return;
  if (full_redraw) {
    drm_clear(attached_drm);
    for (uint64_t i = 0; i < (vt_width * vt_height); i++) {
      vt_draw_char(i);
    }
    full_redraw = false;
    dirty = false;
  }
  if (dirty) {
    // drm_fill_rel_rect(attached_drm, 0, vt_y * 17, vt_width * 8, 34,
    // 0x000000);
    for (uint64_t i = dirty_start; i <= dirty_end; i++) {
      if (i >= vt_width * vt_height)
        break;
      vt_draw_char(i);
    }
    dirty = false;
  }
  dirty_start = vt_width * vt_height;
  dirty_end = 1;
  drm_sync();
}

void vt_advance_y() {
  vt_y++;
  dirty = true;
  if (vt_y >= vt_height) {
    vt_y -= CONFIG_SCROLL_STEP;
    memmove(vt_buffer,
            (void *)((uintptr_t)vt_buffer +
                     sizeof(vt_char_t) * vt_width * CONFIG_SCROLL_STEP),
            vt_width * (vt_height - CONFIG_SCROLL_STEP) * sizeof(vt_char_t));
    memset((void *)((uintptr_t)vt_buffer +
                    sizeof(vt_char_t) *
                        (vt_width * (vt_height - CONFIG_SCROLL_STEP))),
           0, vt_width * sizeof(vt_char_t) * CONFIG_SCROLL_STEP);
    full_redraw = true;
  }
  vt_flush();
}
void vt_advance_x() {
  vt_x++;
  if (vt_x >= vt_width) {
    vt_x = 0;
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
  if (state.working) {
    state = ansi_process(state, c);
  } else if (termcode) {
    switch (c) {
    case 0x01:
      state.gr.font_state = 0;
      state.gr.bg_index = 0;
      state.gr.fg_index = 7;
      break;
    case 0x02:
      state.gr.font_state |= VT_DIM;
      break;
    case 0x03:
      state.gr.font_state |= VT_BOLD;
      break;
    case 0x04:
      state.gr.font_state |= VT_REVERSE;
      break;
    case 0x05:
      state.gr.font_state &= ~VT_DIM;
      break;
    case 0x06:
      state.gr.font_state &= ~VT_BOLD;
      break;
    case 0x07:
      state.gr.font_state &= ~VT_REVERSE;
      break;
    case 0x11:
      state.gr.fg_index = 0;
      break;
    case 0x12:
      state.gr.fg_index = 1;
      break;
    case 0x13:
      state.gr.fg_index = 2;
      break;
    case 0x14:
      state.gr.fg_index = 3;
      break;
    case 0x15:
      state.gr.fg_index = 4;
      break;
    case 0x16:
      state.gr.fg_index = 5;
      break;
    case 0x17:
      state.gr.fg_index = 6;
      break;
    case 0x18:
      state.gr.fg_index = 7;
      break;
    case 0x21:
      state.gr.bg_index = 0;
      break;
    case 0x22:
      state.gr.bg_index = 1;
      break;
    case 0x23:
      state.gr.bg_index = 2;
      break;
    case 0x24:
      state.gr.bg_index = 3;
      break;
    case 0x25:
      state.gr.bg_index = 4;
      break;
    case 0x26:
      state.gr.bg_index = 5;
      break;
    case 0x27:
      state.gr.bg_index = 6;
      break;
    case 0x28:
      state.gr.bg_index = 7;
      break;
    }
    termcode = false;
  } else {
    switch (c) {
    case LF:
      dirty = true;
      dirty_start = min(vt_y * vt_width + vt_x, dirty_start);
      dirty_end = max(vt_y * vt_width + vt_x, dirty_end);
      vt_flush();
      vt_advance_y();
      break;
    case CR:
      vt_x = 0;
      break;
    case BS:
      vt_x--;
      break;
    case ESC:
      state.working = true;
      state.as = C1;
      break;
    case BEL:
      termcode = true;
      break;
    default:
      dirty_start = min(vt_y * vt_width + vt_x, dirty_start);
      dirty_end = max(vt_y * vt_width + vt_x, dirty_end);
      vt_buffer[vt_y * vt_width + vt_x].unicode = c;
      vt_buffer[vt_y * vt_width + vt_x].fg.fb_color = ansi_convert_fg(state.gr);
      vt_buffer[vt_y * vt_width + vt_x].bg.fb_color = ansi_convert_bg(state.gr);
      dirty = true;
      dirty_start = min(vt_y * vt_width + vt_x, dirty_start);
      dirty_end = max(vt_y * vt_width + vt_x, dirty_end);
      vt_advance_x();
      break;
    }
  }
}
