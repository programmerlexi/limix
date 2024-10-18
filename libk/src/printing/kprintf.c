#include <libk/printing.h>
#include <stdarg.h>

char *kfprintf(char *s, ...) {
  va_list args;
  va_start(args, s);
  char *r = kvfprintf(s, args);
  va_end(args);
  return r;
}

void kprintf(char *s, ...) {
  va_list args;
  va_start(args, s);
  kvprintf(s, args);
  va_end(args);
}
