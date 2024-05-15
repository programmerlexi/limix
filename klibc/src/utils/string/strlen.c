#include "types.h"

usz kstrlen(char *s) {
  usz i = 0;
  while (*s) {
    i++;
    s++;
  }
  return i;
}
