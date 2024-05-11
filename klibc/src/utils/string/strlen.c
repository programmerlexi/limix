#include <stddef.h>

size_t kstrlen(char *s) {
  size_t i = 0;
  while (*s) {
    i++;
    s++;
  }
  return i;
}
