#include "math/lib.h"
#include "types.h"
#include "utils/memory/memory.h"
#include "utils/strings/strings.h"
#include <stdbool.h>
#include <stdint.h>

bool kstrnlcmp(char *a, char *b, size_t n, size_t l) {
  if (min(kstrlen(a), l) != min(kstrlen(b), l))
    return false;
  i32 sl = min(min(kstrlen(a), kstrlen(b)), n);
  for (i32 i = 0; i < sl; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
