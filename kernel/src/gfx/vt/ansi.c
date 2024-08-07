#include "kernel/gfx/vt/ansi.h"
#include "kernel/gfx/vt/vt.h"

static u32 _normal_colors[] = {0x000000, 0xd32f2f, 0xafb42b, 0xfbc02d,
                               0x512da8, 0x7b1fa2, 0xfbc02d, 0xeeeeee};

static u32 _dim_colors[] = {0x000000, 0xb71c1c, 0x827717, 0xf57f17,
                            0x311b92, 0x4a148c, 0xf57f17, 0x9e9e9e};

static u32 _bright_colors[] = {0x7f7f7f, 0xf44336, 0xcddc39, 0xffeb3b,
                               0x673ab7, 0x8e24aa, 0xffeb3b, 0xffffff};

static u32 _ansi_convert_fg(VtGraphicRendition gr) {
  if (gr.font_state & VT_BOLD)
    return _bright_colors[gr.fg_index];
  if (gr.font_state & VT_DIM)
    return _dim_colors[gr.fg_index];
  return _normal_colors[gr.fg_index];
}

static u32 _ansi_convert_bg(VtGraphicRendition gr) {
  return _normal_colors[gr.bg_index];
}

u32 ansi_convert_fg(VtGraphicRendition gr) {
  if (gr.font_state & VT_REVERSE)
    return _ansi_convert_bg(gr);
  return _ansi_convert_fg(gr);
}
u32 ansi_convert_bg(VtGraphicRendition gr) {
  if (gr.font_state & VT_REVERSE)
    return _ansi_convert_fg(gr);
  return _ansi_convert_bg(gr);
}

AnsiState ansi_process(AnsiState s, char c) {
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
