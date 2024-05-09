#pragma once

#include <gfx/gfx.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

enum ellipse_type { E_FULL = 0xf, E_1 = 1, E_2 = 2, E_3 = 4, E_4 = 8 };

void fill_rect(absolute64_t x0, absolute64_t y0, offset64_t width,
               offset64_t height, color32_t color);
void draw_line(size_t x0, size_t y0, size_t x1, size_t y1, u32 color);
void draw_circle(size_t x0, size_t y0, size_t radius, u32 color, u8 width);
void draw_ellipse(size_t xm, size_t ym, intmax_t a, intmax_t b, u32 color,
                  u8 type);
void draw_rect(size_t x0, size_t y0, size_t width, size_t height, u32 color);
