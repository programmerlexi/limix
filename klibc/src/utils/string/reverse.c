#include <math/lib.h>
#include <stdbool.h>
#include <stdint.h>
#include <types.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

int reverse(char *s, size_t len) {
  if (!len || !s)
    return STR_NOBUF;
  char c;
  size_t i, j;
  for (i = 0, j = len - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
  return STR_OK;
}
