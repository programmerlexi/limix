#include "libk/printing.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/strings/strings.h"
#include <stdarg.h>
#include <stdbool.h>

void kvprintf(char *s, va_list args) {
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
        kprint(numbuf);
        format = false;
        break;
      case 'w':
        char numbuf1[5];
        kmemset(numbuf1, 0, 5);
        ntos(numbuf1, va_arg(args, int), 16, 4, true, true);
        numbuf1[4] = 0;
        kprint(numbuf1);
        format = false;
        break;
      case 's':
        kprint(va_arg(args, char *));
        format = false;
        break;
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
        kprintc(*s);
        break;
      }
    }
    s++;
  }
}
