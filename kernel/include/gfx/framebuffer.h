#pragma once
#include <boot/limine.h>
#include <stddef.h>
#include <stdint.h>

void putpixel(size_t x, size_t y, uint32_t c);
void putchar16(size_t x, size_t y, char ch, uint32_t color);
void putstr16(size_t x, size_t y, char *s, uint32_t color);
void init_fb(struct limine_framebuffer *);
