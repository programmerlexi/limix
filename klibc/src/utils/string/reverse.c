#include "types.h"
#include "utils/strings/strings.h"

i32 reverse(char *s, usz len) {
  if (!len || !s)
    return STR_NOBUF;
  char c;
  usz i, j;
  for (i = 0, j = len - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
  return STR_OK;
}
