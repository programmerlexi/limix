#include "libk/types.h"

usz kstrnext(char *s, char c) {
  usz i = 0;
  while (*s && *s != c) {
    i++;
    s++;
  }
  return i;
}
