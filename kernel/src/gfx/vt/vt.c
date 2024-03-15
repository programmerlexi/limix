#include "io/serial/serial.h"
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <gfx/vt/ansi.h>
#include <gfx/vt/vt.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/memory/memory.h>

extern void hcf();

vt_char_t *vt_buffer;
uint64_t vt_height;
uint64_t vt_width;
uint64_t vt_x;
uint64_t vt_y;

bool dirty;
bool full_redraw;

ansi_state_t state;

void vt_init() {
  vt_width = g_fb->width / 8;
  vt_height = (g_fb->height - 17) / 17;
  vt_buffer = (vt_char_t *)request_page_block(
      ((vt_width * vt_height * sizeof(vt_char_t)) + 4095) / 4096);
  if (vt_buffer == NULL) {
    serial_writes("[!!] Couldn't initialize VT!\n\r");
    hcf();
  }
  state.gr.font_state = 0;
  state.gr.bg_index = 0;
  state.gr.fg_index = 6;
  vt_clear();
}
void vt_clear() {
  vt_x = 0;
  vt_y = 0;
  memset(vt_buffer, 0, vt_height * vt_width * sizeof(vt_char_t));
  full_redraw = true;
  vt_flush();
}
void vt_draw_char(uint64_t i) {
  vt_char_t c = vt_buffer[i];
  uint64_t cx = i % vt_width;
  uint64_t cy = i / vt_width;
  fill_rect(cx * 8, cy * 17 + 17, 8, 17, c.bg.fb_color);
  putchar16(cx * 8, cy * 17 + 17, c.unicode, c.fg.fb_color);
}
void vt_flush() {
  if (full_redraw) {
    fill_rect(0, 17, vt_width * 8, vt_height * 17, 0x000000);
    for (uint64_t i = 0; i < (vt_width * vt_height); i++) {
      vt_draw_char(i);
    }
    full_redraw = false;
    dirty = false;
  }
  if (dirty) {
    fill_rect(0, vt_y * 17 + 17, vt_width * 8, 34, 0x000000);
    for (uint64_t i = vt_y * vt_width; i < (vt_y + 2) * vt_width; i++) {
      vt_draw_char(i);
    }
  }
}

void vt_advance_y() {
  vt_y++;
  dirty = true;
  if (vt_y >= vt_height) {
    vt_y--;
    memmove(vt_buffer,
            (void *)((uintptr_t)vt_buffer + sizeof(vt_char_t) * vt_width),
            vt_width * (vt_height - 1) * sizeof(vt_char_t));
    memset((void *)((uintptr_t)vt_buffer +
                    sizeof(vt_char_t) * (vt_width * (vt_height - 1))),
           0, vt_width * sizeof(vt_char_t));
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

void kprint(char *s) {
  while (*s) {
    kprintc(*s);
    s++;
  }
  vt_flush();
}
void kprintc(char c) {
  if (state.working) {
    state = ansi_process(state, c);
  } else {
    switch (c) {
    case LF:
      dirty = true;
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
      break;
    default:
      vt_buffer[vt_y * vt_width + vt_x].unicode = c;
      vt_buffer[vt_y * vt_width + vt_x].fg.fb_color = ansi_convert_fg(state.gr);
      vt_buffer[vt_y * vt_width + vt_x].bg.fb_color = ansi_convert_bg(state.gr);
      dirty = true;
      vt_advance_x();
      break;
    }
  }
}
