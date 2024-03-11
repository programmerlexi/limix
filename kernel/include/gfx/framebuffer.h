#pragma once
#include <boot/limine.h>
#include <stdint.h>

void putpixel(int x, int y, uint32_t c);
void putchar16(int x, int y, char ch, uint32_t color);
void putstr16(int x, int y, char *s, uint32_t color);
void init_fb(struct limine_framebuffer *);
