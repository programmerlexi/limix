#include "gfx/vt/vt.h"
#include <gfx/vt/ansi.h>
#include <stdint.h>

uint32_t normal_colors[] = {0x000000, 0xcd0000, 0x00cd00, 0xcdcd00,
                            0x0000ee, 0xcd00cd, 0xe5e5e5};

uint32_t dim_colors[] = {0x000000, 0xaa0000, 0x00aa00, 0xaa5500,
                         0x0000aa, 0xaa00aa, 0xaaaaaa};

uint32_t bright_colors[] = {0x7f7f7f, 0xff0000, 0x00ff00, 0xffff00,
                            0x5c5cff, 0xff00ff, 0xffffff};

uint32_t _ansi_convert_fg(vt_graphic_rendition_t gr) {
  if (gr.font_state & VT_BOLD)
    return bright_colors[gr.fg_index];
  if (gr.font_state & VT_DIM)
    return dim_colors[gr.fg_index];
  return normal_colors[gr.fg_index];
}

uint32_t _ansi_convert_bg(vt_graphic_rendition_t gr) {
  return normal_colors[gr.bg_index];
}

uint32_t ansi_convert_fg(vt_graphic_rendition_t gr) {
  if (gr.font_state & VT_REVERSE)
    return _ansi_convert_bg(gr);
  return _ansi_convert_fg(gr);
}
uint32_t ansi_convert_bg(vt_graphic_rendition_t gr) {
  if (gr.font_state & VT_REVERSE)
    return _ansi_convert_fg(gr);
  return _ansi_convert_bg(gr);
}

ansi_state_t ansi_process(ansi_state_t s, char c) {
  switch (s.as) {
  case C1:
    break;
  case CSI_CMD:
    break;
  case ARG:
    break;
  case SEMICOLON:
    break;
  case CMD:
    break;
  }
  s.working = false;
  s.as = c;
  return s;
}
