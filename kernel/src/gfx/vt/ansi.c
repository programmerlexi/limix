#include <gfx/vt/ansi.h>
#include <gfx/vt/vt.h>
#include <stdint.h>

uint32_t normal_colors[] = {0x000000, 0xd32f2f, 0xafb42b, 0xfbc02d,
                            0x512da8, 0x7b1fa2, 0xfbc02d, 0xeeeeee};

uint32_t dim_colors[] = {0x000000, 0xb71c1c, 0x827717, 0xf57f17,
                         0x311b92, 0x4a148c, 0xf57f17, 0x9e9e9e};

uint32_t bright_colors[] = {0x7f7f7f, 0xf44336, 0xcddc39, 0xffeb3b,
                            0x673ab7, 0x8e24aa, 0xffeb3b, 0xffffff};

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
    switch (c) {
    case '[':
      s.as = CSI_CMD;
      break;
    default:
      s.working = false;
      break;
    }
    break;
  case CSI_CMD:
  case SEMICOLON:
    break;
  case ARG:
    break;
  }
  return s;
}
