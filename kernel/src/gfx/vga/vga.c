#include "gfx/vga.h"
#include "gfx/framebuffer.h"
#include "math/lib.h"
#include <stdint.h>

void fill_rect(size_t x0, size_t y0, size_t width, size_t height, u32 color) {
  for (size_t x = x0; x < x0 + width; x++) {
    for (size_t y = y0; y < y0 + height; y++) {
      putpixel(x, y, color);
    }
  }
}

void draw_ellipse(size_t xm, size_t ym, intmax_t a, intmax_t b, u32 color,
                  u8 type) {
  intmax_t dx = 0, dy = b;
  size_t a2 = a * a, b2 = b * b;
  size_t err = b2 - (2 * b - 1) * a2, e2;

  do {
    if (E_3 & type)
      putpixel(xm + dx, ym + dy, color);
    if (E_4 & type)
      putpixel(xm - dx, ym + dy, color);
    if (E_1 & type)
      putpixel(xm - dx, ym - dy, color);
    if (E_2 & type)
      putpixel(xm + dx, ym - dy, color);
    e2 = 2 * err;
    if (e2 < (2 * dx + 1) * b2) {
      ++dx;
      err += (2 * dx + 1) * b2;
    }
    if (e2 > -(2 * dy - 1) * a2) {
      --dy;
      err -= (2 * dy - 1) * a2;
    }
  } while (dy >= 0);

  while (dx++ < a) {
    putpixel(xm + dx, ym, color);
    putpixel(xm - dx, ym, color);
  }
}

void draw_rect(size_t x0, size_t y0, size_t width, size_t height, u32 color) {
  draw_line(x0, y0, x0 + width, y0, color);
  draw_line(x0, y0, x0, y0 + height, color);
  draw_line(x0 + width, y0, x0 + width, y0 + height, color);
  draw_line(x0, y0 + height, x0 + width, y0 + height, color);
}

void draw_circle(size_t x0, size_t y0, size_t radius, u32 color, u8 width) {
  if (width == 0)
    return;
  i32 f = 1 - radius;
  i32 ddF_x = 0;
  i32 ddF_y = -2 * radius;
  i32 x = 0;
  i32 y = radius;

  putpixel(x0, y0 + radius, color);
  putpixel(x0, y0 - radius, color);
  putpixel(x0 + radius, y0, color);
  putpixel(x0 - radius, y0, color);

  while (x < y) {
    if (f >= 0) {
      y -= 1;
      ddF_y += 2;
      f += ddF_y;
    }
    x += 1;
    ddF_x += 2;
    f += ddF_x + 1;

    putpixel(x0 + x, y0 + y, color);
    putpixel(x0 - x, y0 + y, color);
    putpixel(x0 + x, y0 - y, color);
    putpixel(x0 - x, y0 - y, color);
    putpixel(x0 + y, y0 + x, color);
    putpixel(x0 - y, y0 + x, color);
    putpixel(x0 + y, y0 - x, color);
    putpixel(x0 - y, y0 - x, color);
  }
}

void draw_line(size_t x1, size_t y1, size_t x2, size_t y2, u32 color) {
  i32 x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, deltaslowdirection,
      deltafastdirection, err;
  dx = x2 - x1;
  dy = y2 - y1;
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
  x = x1;
  y = y1;
  err = deltafastdirection / 2;
  putpixel(x, y, color);

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
    putpixel(x, y, color);
  }
}
