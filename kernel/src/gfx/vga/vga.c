#include <kernel/gfx/framebuffer.h>
#include <kernel/gfx/vga.h>
#include <libk/math/lib.h>

void fill_rect(u64 x0, u64 y0, u64 width, u64 height, u32 color) {
  for (usz x = x0; x < x0 + width; x++) {
    for (usz y = y0; y < y0 + height; y++) {
      putpixel(x, y, color);
    }
  }
}

void draw_ellipse(usz xm, usz ym, intmax_t a, intmax_t b, u32 color, u8 type) {
  intmax_t dx = 0, dy = b;
  usz a2 = a * a, b2 = b * b;
  usz err = b2 - (2 * b - 1) * a2, e2;

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

void draw_rect(usz x0, usz y0, usz width, usz height, u32 color) {
  draw_line(x0, y0, x0 + width, y0, color);
  draw_line(x0, y0, x0, y0 + height, color);
  draw_line(x0 + width, y0, x0 + width, y0 + height, color);
  draw_line(x0, y0 + height, x0 + width, y0 + height, color);
}

void draw_circle(usz x0, usz y0, usz radius, u32 color, u8 width) {
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

void draw_line(usz x1, usz y1, usz x2, usz y2, u32 color) {
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
