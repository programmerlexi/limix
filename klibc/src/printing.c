#include <printing.h>
#include <stdarg.h>
#include <utils/strings/strings.h>

void printf(char *s, ...) {
  va_list args;
  va_start(args, s);
  vprintf(s, args);
  va_end(args);
}

void vprintf(char *s, va_list args) {
  bool format = false;
  int base = 0;
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
        char numbuf[256];
        ntos(numbuf, va_arg(args, int), base, 256, !sign, false);
        kprint(numbuf);
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
