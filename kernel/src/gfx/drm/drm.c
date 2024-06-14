#include "kernel/gfx/drm.h"
#include "kernel/config.h"
#include "kernel/fs/devfs.h"
#include "kernel/gfx/font/font.h"
#include "kernel/gfx/framebuffer.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/mm.h"
#include "libk/ipc/spinlock.h"
#include "libk/math/lib.h"
#include "libk/utils/errors.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/memory/safety.h"
#include <stdbool.h>
#include <stddef.h>

static drm_t _drms[MAX_DRMS];
static u64 _active_drm;
static u32 _drm_sys_lock;
static bool drm_initialized;

static u32 _fallback_font[] = {
    0x6999f999, 0xe99e99e0, 0x78888870, 0xe99999e0, 0xf88f88f0, 0xf88f8880,
    0x788b9960, 0x999f9990, 0x44444440, 0x11199960, 0x9ac8ca90, 0x888888f0,
    0x9ff99990, 0x9ddbb990, 0x69999960, 0xe99e8880, 0x6999db70, 0xe99eca90,
    0x788611e0, 0xff666660, 0x99999960, 0x99999f60, 0x9999ff90, 0x99669990,
    0x99966660, 0x0f1248f0, 0x64444460, 0x62222260, 0x22244440, 0x66000660,
    0x00000660, 0x00f00f00, 0x000000f0, 0x69909960, 0x22202220, 0x61168860,
    0x69121960, 0x99961110, 0x68861160, 0x68869960, 0x69901110, 0x69969960,
    0x69961170, 0x00060000};

void drm_init() {
  for (i32 i = 0; i < MAX_DRMS; i++) {
    _drms[i].width = g_fb->width;
    _drms[i].height = g_fb->height;
    _drms[i].framebuffer =
        (u32 *)request_page_block(((g_fb->width * g_fb->pitch) + 4095) / 4096);
    _drms[i].flags = 0;
    _drms[i].bpp = g_fb->bpp;
    _drms[i].pitch = g_fb->pitch;
    if (_drms[i].framebuffer == NULL) {
      kernel_panic_error("DRM init failed");
    }
  }
  _active_drm = 0;
}
static void _drm_sync_real() {
  drm_t ad = _drms[_active_drm];
  kmemcpy(g_fb->address, ad.framebuffer, ad.pitch * ad.height);
}
void drm_switch(u64 drm) {
  if (!drm_initialized)
    return;
  if (drm >= MAX_DRMS)
    return;
  kmemcpy(_drms[_active_drm].framebuffer, g_fb->address,
          _drms[_active_drm].pitch * _drms[_active_drm].height);
  spinlock(&_drm_sys_lock);
  _active_drm = drm;
  _drm_sync_real();
  spinunlock(&_drm_sys_lock);
}

void drm_sync() {
  if (!drm_initialized)
    return;
  spinlock(&_drm_sys_lock);
#ifndef DRM_WRITETHROUGH
  _drm_sync_real();
#endif
  spinunlock(&_drm_sys_lock);
}
void drm_plot(u64 drm, u64 x, u64 y, u32 c) {
  if (!drm_initialized)
    return;
  if (x > _drms[drm].width)
    return;
  if (y > _drms[drm].height)
    return;
  spinlock(&_drms[drm].lock);
#ifndef DRM_WRITETHROUGH
  drms[drm].framebuffer[drms[drm].width * y + x] = c;
#else
  if (_active_drm == drm) {
    spinlock(&_drm_sys_lock);
    *(u32 *)((uptr)g_fb->address +
             (_drms[drm].pitch * y + x * _drms[drm].bpp / 8)) = c;
    spinunlock(&_drm_sys_lock);
  } else
    *(u32 *)((uptr)_drms[drm].framebuffer +
             (_drms[drm].pitch * y + x * _drms[drm].bpp / 8)) = c;
#endif
  spinunlock(&_drms[drm].lock);
}
void drm_plot_line(u64 drm, u64 x0, u64 y0, u64 x1, u64 y1, u32 c) {
  if (!drm_initialized)
    return;
  i32 x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, deltaslowdirection,
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
void drm_plot_rect(u64 drm, u64 x0, u64 y0, u64 x1, u64 y1, u32 c) {
  if (!drm_initialized)
    return;
  drm_plot_line(drm, x0, y0, x1, y0, c);
  drm_plot_line(drm, x0, y0, x0, y1, c);
  drm_plot_line(drm, x0, y1, x1, y1, c);
  drm_plot_line(drm, x1, y0, x1, y1, c);
}
void drm_plot_circle(u64 drm, u64 x0, u64 y0, u64 r, u32 c) {
  if (!drm_initialized)
    return;
  i32 f = 1 - r;
  i32 ddF_x = 0;
  i32 ddF_y = -2 * r;
  i32 x = 0;
  i32 y = r;

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
void drm_fill_rect(u64 drm, u64 x0, u64 y0, u64 x1, u64 y1, u32 c) {
  if (!drm_initialized)
    return;
  for (u64 x = x0; x < x1; x++) {
    for (u64 y = y0; y < y1; y++) {
      drm_plot(drm, x, y, c);
    }
  }
}
void drm_fill_rel_rect(u64 drm, u64 x0, u64 y0, u64 w, u64 h, u32 c) {
  if (!drm_initialized)
    return;
  for (u64 x = x0; x < x0 + w; x++) {
    for (u64 y = y0; y < y0 + h; y++) {
      drm_plot(drm, x, y, c);
    }
  }
}
void drm_clear(u64 drm) {
  if (!drm_initialized)
    return;
  spinlock(&_drm_sys_lock);
  kmemset(_drms[drm].framebuffer, 0,
          _drms[drm].width * _drms[drm].height * _drms[drm].bpp / 8);
  spinunlock(&_drm_sys_lock);
  drm_sync();
}
static void _drm_plot_char(u64 drm, u64 x, u64 y, u32 ch, u32 c) {
  for (u8 hi = 0; hi < 16; hi++) {
    u8 m = 0x80;
    for (u8 i = 0; i < 8; i++) {
      if (g_8x16_font[16 * ch + hi] & m)
        drm_plot(drm, x + i, y + hi, c);
      m >>= 1;
    }
  }
}
static void _drm_plot_char_solid(u64 drm, u64 x, u64 y, u32 ch, u32 c, u32 b) {
  for (u8 hi = 0; hi < 16; hi++) {
    u8 m = 0x80;
    for (u8 i = 0; i < 8; i++) {
      if (g_8x16_font[16 * ch + hi] & m)
        drm_plot(drm, x + i, y + hi, c);
      else
        drm_plot(drm, x + i, y + hi, b);
      m >>= 1;
    }
  }
}

static u32 _drm_get_fallback(u32 c) {
  if (c >= 'A' && c <= 'Z')
    return _fallback_font[c - 'A'];
  if (c >= 'a' && c <= 'z')
    return _fallback_font[c - 'a'];
  if (c == '[')
    return _fallback_font['Z' - 'A' + 1];
  if (c == ']')
    return _fallback_font['Z' - 'A' + 2];
  if (c == '/')
    return _fallback_font['Z' - 'A' + 3];
  if (c == ':')
    return _fallback_font['Z' - 'A' + 4];
  if (c == '.')
    return _fallback_font['Z' - 'A' + 5];
  if (c == '=')
    return _fallback_font['Z' - 'A' + 6];
  if (c == '_')
    return _fallback_font['Z' - 'A' + 7];
  if (c >= '0' && c <= '9')
    return _fallback_font['Z' - 'A' + 8 + c - '0'];
  if (c == '-')
    return _fallback_font['Z' - 'A' + 18];
  if (c == ' ')
    return 0;
  return c;
}

static void _drm_plot_char_fallback(u64 drm, u64 x, u64 y, u32 ch, u32 c) {
  for (u8 hi = 0; hi < 8; hi++) {
    u8 m = 0x80;
    for (u8 i = 0; i < 4; i++) {
      if ((_drm_get_fallback(ch) >> ((7 - hi) * 4)) & m)
        drm_fill_rel_rect(drm, x + i * 2, y + hi * 2, 2, 2, c);
      m >>= 1;
    }
  }
}
static void _drm_plot_char_solid_fallback(u64 drm, u64 x, u64 y, u32 ch, u32 c,
                                          u32 b) {
  for (u8 hi = 0; hi < 8; hi++) {
    u8 m = 0x80;
    for (u8 i = 0; i < 4; i++) {
      if ((_drm_get_fallback(ch) >> ((7 - hi) * 4)) & m)
        drm_fill_rel_rect(drm, x + (i * 2), y + (hi * 2), 2, 2, c);
      else
        drm_fill_rel_rect(drm, x + (i * 2), y + (hi * 2), 2, 2, b);
      m >>= 1;
    }
  }
}

void drm_plot_char(u64 drm, u64 x, u64 y, u32 ch, u32 c) {
  if (!drm_initialized)
    return;
  if (g_8x16_font)
    _drm_plot_char(drm, x, y, ch, c);
  else
    _drm_plot_char_fallback(drm, x, y, ch, c);
}
void drm_plot_char_solid(u64 drm, u64 x, u64 y, u32 ch, u32 c, u32 b) {
  if (g_8x16_font)
    _drm_plot_char_solid(drm, x, y, ch, c, b);
  else
    _drm_plot_char_solid_fallback(drm, x, y, ch, c, b);
}

u64 drm_width(u64 drm) { return _drms[drm].width; }
u64 drm_height(u64 drm) { return _drms[drm].height; }

bool drm_is_attached_to_process(u64 drm) {
  if (!drm_initialized)
    return false;
  return _drms[drm].flags & DRM_ATTACHED_TO_PROCESS;
}

static i32 _drm_write(void *d, u64 o, u64 s, char *b) {
  drm_number_t *dn = (drm_number_t *)d;
  if (o > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  if (o + s > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  for (u64 i = 0; i < s; i++) {
    drm_plot(*dn, (o + i) % drm_width(*dn), (o + i) / drm_width(*dn),
             ((u32 *)b)[i]);
  }
  return E_SUCCESS;
}

static i32 _drm_read(void *d, u64 o, u64 s, char *b) {
  drm_number_t *dn = (drm_number_t *)d;
  if (o > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  if (o + s > drm_width(*dn) * drm_height(*dn))
    return E_OUTOFBOUNDS;
  for (u64 i = 0; i < s; i++) {
    if (_active_drm == *dn)
      ((u32 *)b)[i] = ((u32 *)g_fb->address)[o * i];
    else
      ((u32 *)b)[i] = _drms[*dn].framebuffer[o + i];
  }
  return E_SUCCESS;
}

void drm_register_vfs() {
  if (!drm_initialized)
    return;
  for (u64 i = 0; i < MAX_DRMS; i++) {
    drm_number_t *dn = (drm_number_t *)kmalloc(sizeof(drm_number_t));
    nullsafe_error(dn, "Out of memory");
    char *devname = (char *)kmalloc(4);
    nullsafe_error(devname, "Out of memory");
    kmemcpy(devname, "drm", 3);
    devname[3] = 'a' + i;
    *dn = i;
    devfs_create(devname, _drm_read, _drm_write, dn);
    kfree(devname);
  }
  devfs_reload();
}
