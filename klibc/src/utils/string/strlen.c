#include <stddef.h>

size_t strlen(char *s) {
  size_t i = 0;
  while (*s) {
    i++;
    s++;
  }
  return i;
}
