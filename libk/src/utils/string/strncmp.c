#include "libk/math/lib.h"
#include "libk/utils/strings/strings.h"

bool kstrncmp(char *a, char *b, usz n) {
  if (min(kstrlen(a), n) != min(kstrlen(b), n))
    return false;
  i32 l = min(min(kstrlen(a), kstrlen(b)), n);
  for (i32 i = 0; i < l; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
