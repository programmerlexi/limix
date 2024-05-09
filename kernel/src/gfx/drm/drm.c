#include <config.h>
#include <fs/devfs.h>
#include <gfx/drm.h>
#include <gfx/font/font.h>
#include <gfx/framebuffer.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <kipc/spinlock.h>
#include <math/lib.h>
#include <mm/heap.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/memory.h>
#include <utils/memory/safety.h>

static drm_t drms[MAX_DRMS];
static uint64_t active_drm;
static uint32_t drm_sys_lock;

static uint32_t fallback_font[] = {
    0x6999f999, 0xe99e99e0, 0x78888870, 0xe99999e0, 0xf88f88f0, 0xf88f8880,
    0x788b9960, 0x999f9990, 0x44444440, 0x11199960, 0x9ac8ca90, 0x888888f0,
    0x9ff99990, 0x9ddbb990, 0x69999960, 0xe99e8880, 0x6999db70, 0xe99eca90,
    0x788611e0, 0xff666660, 0x99999960, 0x99999f60, 0x9999ff90, 0x99669990,
    0x99966660, 0x0f1248f0, 0x64444460, 0x62222260, 0x22244440, 0x66000660,
    0x00000660, 0x00f00f00, 0x000000f0, 0x69909960, 0x22202220, 0x61168860,
    0x69121960, 0x99961110, 0x68861160, 0x68869960, 0x69901110, 0x69969960,
    0x69961170, 0x00060000};

void drm_init() {
  for (int i = 0; i < MAX_DRMS; i++) {
    drms[i].width = g_fb->width;
    drms[i].height = g_fb->height;
    drms[i].framebuffer =
        request_page_block(((g_fb->width * g_fb->height * 4) + 4095) / 4096);
    drms[i].flags = 0;
    if (drms[i].framebuffer == NULL) {
      kernel_panic_error("DRM init failed");
    }
  }
  active_drm = 0;
}
static void _drm_sync_real() {
  drm_t ad = drms[active_drm];
  memcpy(g_fb->address, ad.framebuffer, ad.width * ad.height * 4);
}
void drm_switch(uint64_t drm) {
  if (drm >= MAX_DRMS)
    return;
  memcpy(drms[active_drm].framebuffer, g_fb->address,
         drms[active_drm].width * drms[active_drm].height * 4);
  spinlock(&drm_sys_lock);
  active_drm = drm;
  _drm_sync_real();
  spinunlock(&drm_sys_lock);
}

void drm_sync() {
  spinlock(&drm_sys_lock);
#ifndef DRM_WRITETHROUGH
  _drm_sync_real();
#endif
  spinunlock(&drm_sys_lock);
}
void drm_plot(uint64_t drm, uint64_t x, uint64_t y, uint32_t c) {
  if (x > drms[drm].width)
    return;
  if (y > drms[drm].height)
    return;
  spinlock(&drms[drm].lock);
#ifndef DRM_WRITETHROUGH
  drms[drm].framebuffer[drms[drm].width * y + x] = c;
#else
  if (active_drm == drm) {
    spinlock(&drm_sys_lock);
    ((uint32_t *)g_fb->address)[drms[drm].width * y + x] = c;
    spinunlock(&drm_sys_lock);
  } else
    drms[drm].framebuffer[drms[drm].width * y + x] = c;
#endif
  spinunlock(&drms[drm].lock);
}
void drm_plot_line(uint64_t drm, uint64_t x0, uint64_t y0, uint64_t x1,
                   uint64_t y1, uint32_t c) {
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
  drm_plot(drm, x, y, c);

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
    drm_plot(drm, x, y, c);
  }
}
void drm_plot_rect(uint64_t drm, uint64_t x0, uint64_t y0, uint64_t x1,
                   uint64_t y1, uint32_t c) {
  drm_plot_line(drm, x0, y0, x1, y0, c);
  drm_plot_line(drm, x0, y0, x0, y1, c);
  drm_plot_line(drm, x0, y1, x1, y1, c);
  drm_plot_line(drm, x1, y0, x1, y1, c);
}
void drm_plot_circle(uint64_t drm, uint64_t x0, uint64_t y0, uint64_t r,
                     uint32_t c) {
  int f = 1 - r;
  int ddF_x = 0;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drm_plot(drm, x0, y0 + r, c);
  drm_plot(drm, x0, y0 - r, c);
  drm_plot(drm, x0 + r, y0, c);
  drm_plot(drm, x0 - r, y0, c);

  while (x < y) {
    if (f >= 0) {
      y -= 1;
      ddF_y += 2;
      f += ddF_y;
    }
    x += 1;
    ddF_x += 2;
    f += ddF_x + 1;

    drm_plot(drm, x0 + x, y0 + y, c);
    drm_plot(drm, x0 - x, y0 + y, c);
    drm_plot(drm, x0 + x, y0 - y, c);
    drm_plot(drm, x0 - x, y0 - y, c);
    drm_plot(drm, x0 + y, y0 + x, c);
    drm_plot(drm, x0 - y, y0 + x, c);
    drm_plot(drm, x0 + y, y0 - x, c);
    drm_plot(drm, x0 - y, y0 - x, c);
  }
}
void drm_fill_rect(uint64_t drm, uint64_t x0, uint64_t y0, uint64_t x1,
                   uint64_t y1, uint32_t c) {
  for (uint64_t x = x0; x < x1; x++) {
    for (uint64_t y = y0; y < y1; y++) {
      drm_plot(drm, x, y, c);
    }
  }
}
void drm_fill_rel_rect(uint64_t drm, uint64_t x0, uint64_t y0, uint64_t w,
                       uint64_t h, uint32_t c) {
  for (uint64_t x = x0; x < x0 + w; x++) {
    for (uint64_t y = y0; y < y0 + h; y++) {
      drm_plot(drm, x, y, c);
    }
  }
}
void drm_clear(uint64_t drm) {
  spinlock(&drm_sys_lock);
  memset(drms[drm].framebuffer, 0, drms[drm].width * drms[drm].height * 4);
  spinunlock(&drm_sys_lock);
  drm_sync();
}
static void _drm_plot_char(uint64_t drm, uint64_t x, uint64_t y, uint32_t ch,
                           uint32_t c) {
  for (uint8_t hi = 0; hi < 16; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 8; i++) {
      if (g_8x16_font[16 * ch + hi] & m)
        drm_plot(drm, x + i, y + hi, c);
      m >>= 1;
    }
  }
}
static void _drm_plot_char_solid(uint64_t drm, uint64_t x, uint64_t y,
                                 uint32_t ch, uint32_t c, uint32_t b) {
  for (uint8_t hi = 0; hi < 16; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 8; i++) {
      if (g_8x16_font[16 * ch + hi] & m)
        drm_plot(drm, x + i, y + hi, c);
      else
        drm_plot(drm, x + i, y + hi, b);
      m >>= 1;
    }
  }
}

static uint32_t _drm_get_fallback(uint32_t c) {
  if (c >= 'A' && c <= 'Z')
    return fallback_font[c - 'A'];
  if (c >= 'a' && c <= 'z')
    return fallback_font[c - 'a'];
  if (c == '[')
    return fallback_font['Z' - 'A' + 1];
  if (c == ']')
    return fallback_font['Z' - 'A' + 2];
  if (c == '/')
    return fallback_font['Z' - 'A' + 3];
  if (c == ':')
    return fallback_font['Z' - 'A' + 4];
  if (c == '.')
    return fallback_font['Z' - 'A' + 5];
  if (c == '=')
    return fallback_font['Z' - 'A' + 6];
  if (c == '_')
    return fallback_font['Z' - 'A' + 7];
  if (c >= '0' && c <= '9')
    return fallback_font['Z' - 'A' + 8 + c - '0'];
  if (c == '-')
    return fallback_font['Z' - 'A' + 18];
  if (c == ' ')
    return 0;
  return c;
}

static void _drm_plot_char_fallback(uint64_t drm, uint64_t x, uint64_t y,
                                    uint32_t ch, uint32_t c) {
  for (uint8_t hi = 0; hi < 8; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 4; i++) {
      if ((_drm_get_fallback(ch) >> ((7 - hi) * 4)) & m)
        drm_fill_rel_rect(drm, x + i * 2, y + hi * 2, 2, 2, c);
      m >>= 1;
    }
  }
}
static void _drm_plot_char_solid_fallback(uint64_t drm, uint64_t x, uint64_t y,
                                          uint32_t ch, uint32_t c, uint32_t b) {
  for (uint8_t hi = 0; hi < 8; hi++) {
    uint8_t m = 0x80;
    for (uint8_t i = 0; i < 4; i++) {
      if ((_drm_get_fallback(ch) >> ((7 - hi) * 4)) & m)
        drm_fill_rel_rect(drm, x + (i * 2), y + (hi * 2), 2, 2, c);
      else
        drm_fill_rel_rect(drm, x + (i * 2), y + (hi * 2), 2, 2, b);
      m >>= 1;
    }
  }
}

void drm_plot_char(uint64_t drm, uint64_t x, uint64_t y, uint32_t ch,
                   uint32_t c) {
  if (g_8x16_font)
    _drm_plot_char(drm, x, y, ch, c);
  else
    _drm_plot_char_fallback(drm, x, y, ch, c);
}
void drm_plot_char_solid(uint64_t drm, uint64_t x, uint64_t y, uint32_t ch,
                         uint32_t c, uint32_t b) {
  if (g_8x16_font)
    _drm_plot_char_solid(drm, x, y, ch, c, b);
  else
    _drm_plot_char_solid_fallback(drm, x, y, ch, c, b);
}

uint64_t drm_width(uint64_t drm) { return drms[drm].width; }
uint64_t drm_height(uint64_t drm) { return drms[drm].height; }

bool drm_is_attached_to_process(uint64_t drm) {
  return drms[drm].flags & DRM_ATTACHED_TO_PROCESS;
}

static int _drm_write(void *d, uint64_t o, uint64_t s, char *b) {
  drm_number_t *dn = d;
  if (o > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  if (o + s > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  for (uint64_t i = 0; i < s; i++) {
    drm_plot(*dn, (o + i) % drm_width(*dn), (o + i) / drm_width(*dn),
             ((uint32_t *)b)[i]);
  }
  return E_SUCCESS;
}

static int _drm_read(void *d, uint64_t o, uint64_t s, char *b) {
  drm_number_t *dn = d;
  if (o > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  if (o + s > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  for (uint64_t i = 0; i < s; i++) {
    if (active_drm == *dn)
      ((uint32_t *)b)[i] = ((uint32_t *)g_fb->address)[o * i];
    else
      ((uint32_t *)b)[i] = drms[*dn].framebuffer[o + i];
  }
  return E_SUCCESS;
}

void drm_register_vfs() {
  for (uint64_t i = 0; i < MAX_DRMS; i++) {
    drm_number_t *dn = malloc(sizeof(drm_number_t));
    nullsafe_error(dn, "Out of memory");
    char *devname = malloc(4);
    nullsafe_error(devname, "Out of memory");
    memcpy(devname, "drm", 3);
    devname[3] = 'a' + i;
    *dn = i;
    devfs_create(devname, _drm_read, _drm_write, dn);
    free(devname);
  }
  devfs_reload();
}
