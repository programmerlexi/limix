#include <math/lib.h>
#include <stdbool.h>
#include <stdint.h>
#include <types.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

bool strnlcmp(char *a, char *b, size_t n, size_t l) {
  if (min(strlen(a), l) != min(strlen(b), l))
    return false;
  int sl = min(min(strlen(a), strlen(b)), n);
  for (int i = 0; i < sl; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
