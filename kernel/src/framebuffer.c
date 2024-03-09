#include <framebuffer.h>
#include <limine.h>
#include <stdint.h>
#include <vga_font.h>

struct limine_framebuffer *g_fb;

void init_fb(struct limine_framebuffer *fb) { g_fb = fb; }

void putpixel(int x, int y, uint32_t c) {
  ((uint32_t *)g_fb->address)[y * g_fb->pitch / 4 + x] = c;
}

void _internal_putchar(int x, int y, uint8_t ch, uint32_t color, uint8_t h,
                       uint8_t *font) {
  for (uint8_t hi = 0; hi < h; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 8; i++) {
      if (font[h * ch + hi] & m)
        putpixel(x + i, y + hi, color);
      m >>= 1;
    }
  }
}

void putchar16(int x, int y, char ch, uint32_t color) {
  _internal_putchar(x, y, (uint8_t)ch, color, 16, g_8x16_font);
}

void putstr16(int x, int y, char *s, uint32_t color) {
  int xo = 0;
  while (*s) {
    putchar16(x + xo, y, *s, color);
    s++;
    xo += 8;
  }
}
