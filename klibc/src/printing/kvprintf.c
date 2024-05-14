#include "printing.h"
#include "utils/strings/strings.h"
#include <stdarg.h>
#include <stdint.h>

void kvprintf(char *s, va_list args) {
  bool format = false;
  i32 base = 0;
  bool sign = true;
  while (*s) {
    if (format) {
      switch (*s) {
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
        char numbuf[128];
        ntos(numbuf, va_arg(args, i64), base, 128, !sign, false);
        kprint(numbuf);
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
        break;
      default:
        kprintc(*s);
        break;
      }
    }
    s++;
  }
}
