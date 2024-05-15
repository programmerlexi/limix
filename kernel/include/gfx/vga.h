#pragma once

#include "gfx/gfx.h"
#include "types.h"

enum ellipse_type { E_FULL = 0xf, E_1 = 1, E_2 = 2, E_3 = 4, E_4 = 8 };

void fill_rect(absolute64_t x0, absolute64_t y0, offset64_t width,
               offset64_t height, color32_t color);
void draw_line(usz x0, usz y0, usz x1, usz y1, u32 color);
void draw_circle(usz x0, usz y0, usz radius, u32 color, u8 width);
void draw_ellipse(usz xm, usz ym, isz a, isz b, u32 color, u8 type);
void draw_rect(usz x0, usz y0, usz width, usz height, u32 color);
