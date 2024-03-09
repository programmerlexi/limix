#pragma once

#include <stdint.h>

enum ellipse_type {
    E_FULL = 0xf,
    E_1 = 1,
    E_2 = 2,
    E_3 = 4,
    E_4 = 8
};

void fill_rect(uint64_t x0, uint64_t y0, uint64_t width, uint64_t height, uint32_t color);
void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
void draw_circle(int x0, int y0, int radius,uint32_t color,uint8_t width);
void draw_ellipse(int xm, int ym, int a, int b, uint32_t color,uint8_t type);
void draw_rect(uint64_t x0, uint64_t y0, uint64_t width, uint64_t height, uint32_t color);
