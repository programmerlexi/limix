#include <kernel/mm/heap.h>
#include <libk/printing.h>
#include <libk/utils/memory/heap_wrap.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/strings/strings.h>
#include <stdarg.h>
#include <stdbool.h>

void kvprintf(char *s, va_list args) {
  char *p = kvfprintf(s, args);
  kprint(p);
  kfree(p);
}

static char *_string_op(char *s, u64 sl, u64 *dl) {
  char *n = s;
  while (*dl < sl + 4) {
    u64 l = *dl + 16;
    n = krealloc(n, l, *dl);
    *dl = l;
  }
  return n;
}

char *kvfprintf(char *s, va_list args) {
  char *r = kzalloc(16);
  u64 rl = 0;
  u64 dl = 16;
  bool format = false;
  i32 base = 0;
  bool sign = true;
  bool pad = false;
  while (*s) {
    if (format) {
      switch (*s) {
      case 'l':
        pad = true;
      case 'x':
        base += 6;
      case 'u':
        sign = false;
      case 'i':
        base += 2;
      case 'o':
        base += 6;
      case 'd':
        base += 2;
        if (base == 2)
          sign = false;
        char numbuf[129];
        kmemset(numbuf, 0, 129);
        u64 l = 16;
        switch (base) {
        case 2:
          l = 128;
          break;
        case 8:
          l = 32;
          break;
        case 10:
          l = 30;
          break;
        }
        ntos(numbuf, va_arg(args, i64), base, l, !sign, pad);
        r = _string_op(r, rl + kstrlen(numbuf), &dl);
        kmemcpy(&r[rl], numbuf, kstrlen(numbuf));
        rl += kstrlen(numbuf);
        format = false;
        break;
      case 'w': {
        char numbuf1[4];
        ntos(numbuf1, va_arg(args, int), 16, 4, true, true);
        r = _string_op(r, rl + 4, &dl);
        kmemcpy(&r[rl], numbuf1, 4);
        rl += 4;
        format = false;
        break;
      }
      case 's': {
        char *aps = va_arg(args, char *);
        r = _string_op(r, rl + kstrlen(aps), &dl);
        kmemcpy(&r[rl], aps, kstrlen(aps));
        rl += kstrlen(aps);
        format = false;
        break;
      }
      }
    } else {
      switch (*s) {
      case '%':
        format = true;
        base = 0;
        sign = true;
        pad = false;
        break;
      default:
        rl++;
        r = _string_op(r, rl + 1, &dl);
        r[rl - 1] = *s;
        break;
      }
    }
    s++;
  }
  r = _string_op(r, rl + 2, &dl);
  r[rl] = 0;
  return r;
}
