#include "defines.h"
#include "printing.h"
#include "utils/strings/strings.h"
#include <stdarg.h>

void kvprintf(char *s, va_list args) {
  BOOL format = FALSE;
  i32 base = 0;
  BOOL sign = TRUE;
  while (*s) {
    if (format) {
      switch (*s) {
      case 'x':
        base += 6;
      case 'u':
        sign = FALSE;
      case 'i':
        base += 2;
      case 'o':
        base += 6;
      case 'd':
        base += 2;
        if (base == 2)
          sign = FALSE;
        char numbuf[128];
        ntos(numbuf, va_arg(args, i64), base, 128, !sign, FALSE);
        kprint(numbuf);
        format = FALSE;
        break;
      case 's':
        kprint(va_arg(args, char *));
        format = FALSE;
        break;
      }
    } else {
      switch (*s) {
      case '%':
        format = TRUE;
        base = 0;
        sign = TRUE;
        break;
      default:
        kprintc(*s);
        break;
      }
    }
    s++;
  }
}
