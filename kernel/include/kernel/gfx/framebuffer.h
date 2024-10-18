#pragma once

#include <kernel/gfx/gfx.h>
#include <limine.h>

extern struct limine_framebuffer *g_fb;

void putpixel(absolute64_t x, absolute64_t y, color32_t c);
void putchar16(absolute64_t x, absolute64_t y, char character, color32_t color);
void putstr16(absolute64_t x, absolute64_t y, char *str, color32_t color);
void fb_init(struct limine_framebuffer *);
