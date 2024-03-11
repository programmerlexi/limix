#pragma once

#include <stddef.h>
#include <stdint.h>

enum ellipse_type { E_FULL = 0xf, E_1 = 1, E_2 = 2, E_3 = 4, E_4 = 8 };

void fill_rect(size_t x0, size_t y0, size_t width, size_t height,
               uint32_t color);
void draw_line(size_t x1, size_t y1, size_t x2, size_t y2, uint32_t color);
void draw_circle(size_t x0, size_t y0, size_t radius, uint32_t color,
                 uint8_t width);
void draw_ellipse(size_t xm, size_t ym, intmax_t a, intmax_t b, uint32_t color,
                  uint8_t type);
void draw_rect(size_t x0, size_t y0, size_t width, size_t height,
               uint32_t color);
