#include <boot/limine.h>
#include <gfx/font/font.h>
#include <gfx/framebuffer.h>
#include <stdint.h>

struct limine_framebuffer *g_fb;

void fb_init(struct limine_framebuffer *fb) { g_fb = fb; }

void putpixel(size_t x, size_t y, u32 c) {
  ((u32 *)g_fb->address)[y * g_fb->pitch / 4 + x] = c;
}

static void _internal_putchar(size_t x, size_t y, u8 ch, u32 color, u8 h,
                              u8 *font) {
  for (u8 hi = 0; hi < h; hi++) {
    u8 m = 0x80;
    for (u8 i = 0; i < 8; i++) {
      if (font[h * ch + hi] & m)
        putpixel(x + i, y + hi, color);
      m >>= 1;
    }
  }
}

void putchar16(size_t x, size_t y, char ch, u32 color) {
  if (g_8x16_font)
    _internal_putchar(x, y, (u8)ch, color, 16, g_8x16_font);
}

void putstr16(size_t x, size_t y, char *s, u32 color) {
  i32 xo = 0;
  while (*s) {
    putchar16(x + xo, y, *s, color);
    s++;
    xo += 8;
  }
}
