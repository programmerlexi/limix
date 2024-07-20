#include "libk/types.h"
#include "libk/utils/memory/safety.h"

usz kstrlen(char *s) {
  nullsafe_with(s, return 0);
  usz i = 0;
  while (*s) {
    i++;
    s++;
  }
  return i;
}
