#include <printing.h>
#include <stdarg.h>

void kprintf(char *s, ...) {
  va_list args;
  va_start(args, s);
  kvprintf(s, args);
  va_end(args);
}
