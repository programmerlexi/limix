#include <config.h>
#include <gfx/drm.h>
#include <gfx/framebuffer.h>
#include <gfx/vga_font.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <math/lib.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/memory/memory.h>

drm_t drms[MAX_DRMS];
uint64_t active_drm;

void drm_init() {
  for (int i = 0; i < MAX_DRMS; i++) {
    drms[i].width = g_fb->width;
    drms[i].height = g_fb->height;
    drms[i].framebuffer =
        request_page_block(((g_fb->width * g_fb->height * 4) + 4095) / 4096);
    drms[i].flags = 0;
    if (drms[i].framebuffer == NULL) {
      serial_writes("[!!] DRM init failed\n\r");
      hcf();
    }
  }
  active_drm = 0;
}
void drm_switch(uint64_t drm) {
  if (drm >= MAX_DRMS)
    return;
  active_drm = drm;
  drm_sync();
}
void drm_sync() {
  drm_t ad = drms[active_drm];
  memcpy(g_fb->address, ad.framebuffer, ad.width * ad.height * 4);
}
void drm_plot(uint64_t x, uint64_t y, uint32_t c) {
  if (x > drms[active_drm].width)
    return;
  if (y > drms[active_drm].height)
    return;
  drms[active_drm].framebuffer[drms[active_drm].width * y + x] = c;
}
void drm_plot_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1,
                   uint32_t c) {
  int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, deltaslowdirection,
      deltafastdirection, err;
  dx = x1 - x0;
  dy = y1 - y0;
  incx = sign(dx);
  incy = sign(dy);
  if (dx < 0)
    dx *= -1;
  if (dy < 0)
    dy *= -1;
  if (dx > dy) {
    pdx = incx;
    pdy = 0;
    ddx = incx;
    ddy = incy;
    deltaslowdirection = dy;
    deltafastdirection = dx;
  } else {
    pdx = 0;
    pdy = incy;
    ddx = incx;
    ddy = incy;
    deltaslowdirection = dx;
    deltafastdirection = dy;
  }
  x = x0;
  y = y0;
  err = deltafastdirection / 2;
  drm_plot(x, y, c);

  for (t = 0; t < deltafastdirection; ++t) {
    err -= deltaslowdirection;
    if (err < 0) {
      err += deltafastdirection;
      x += ddx;
      y += ddy;
    } else {
      x += pdx;
      y += pdy;
    }
    drm_plot(x, y, c);
  }
}
void drm_plot_rect(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1,
                   uint32_t c) {
  drm_plot_line(x0, y0, x1, y0, c);
  drm_plot_line(x0, y0, x0, y1, c);
  drm_plot_line(x0, y1, x1, y1, c);
  drm_plot_line(x1, y0, x1, y1, c);
}
void drm_plot_circle(uint64_t x0, uint64_t y0, uint64_t r, uint32_t c) {
  int f = 1 - r;
  int ddF_x = 0;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drm_plot(x0, y0 + r, c);
  drm_plot(x0, y0 - r, c);
  drm_plot(x0 + r, y0, c);
  drm_plot(x0 - r, y0, c);

  while (x < y) {
    if (f >= 0) {
      y -= 1;
      ddF_y += 2;
      f += ddF_y;
    }
    x += 1;
    ddF_x += 2;
    f += ddF_x + 1;

    drm_plot(x0 + x, y0 + y, c);
    drm_plot(x0 - x, y0 + y, c);
    drm_plot(x0 + x, y0 - y, c);
    drm_plot(x0 - x, y0 - y, c);
    drm_plot(x0 + y, y0 + x, c);
    drm_plot(x0 - y, y0 + x, c);
    drm_plot(x0 + y, y0 - x, c);
    drm_plot(x0 - y, y0 - x, c);
  }
}
void drm_fill_rect(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1,
                   uint32_t c) {
  for (uint64_t x = x0; x < x1; x++) {
    for (uint64_t y = y0; y < y1; y++) {
      drm_plot(x, y, c);
    }
  }
}
void drm_fill_rel_rect(uint64_t x0, uint64_t y0, uint64_t w, uint64_t h,
                       uint32_t c) {
  for (uint64_t x = x0; x < x0 + w; x++) {
    for (uint64_t y = y0; y < y0 + h; y++) {
      drm_plot(x, y, c);
    }
  }
}
void drm_clear() {
  memset(&(drms[active_drm].framebuffer), 0,
         drms[active_drm].width * drms[active_drm].height * 4);
}
void drm_plot_char(uint64_t x, uint64_t y, uint32_t ch, uint32_t c) {
  for (uint8_t hi = 0; hi < 16; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 8; i++) {
      if (g_8x16_font[16 * ch + hi] & m)
        drm_plot(x + i, y + hi, c);
      m >>= 1;
    }
  }
}

uint64_t drm_width() { return drms[active_drm].width; }
uint64_t drm_height() { return drms[active_drm].height; }
