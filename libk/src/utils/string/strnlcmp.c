#include "libk/math/lib.h"
#include "libk/types.h"
#include "libk/utils/strings/strings.h"
#include <stdbool.h>

bool kstrnlcmp(char *a, char *b, usz n, usz l) {
  if (min(kstrlen(a), l) != min(kstrlen(b), l))
    return false;
  i32 sl = min(min(kstrlen(a), kstrlen(b)), n);
  for (i32 i = 0; i < sl; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
