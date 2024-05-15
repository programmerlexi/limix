#include "defines.h"
#include "math/lib.h"
#include "types.h"
#include "utils/strings/strings.h"

BOOL kstrnlcmp(char *a, char *b, usz n, usz l) {
  if (min(kstrlen(a), l) != min(kstrlen(b), l))
    return FALSE;
  i32 sl = min(min(kstrlen(a), kstrlen(b)), n);
  for (i32 i = 0; i < sl; i++) {
    if (a[i] != b[i])
      return FALSE;
  }
  return TRUE;
}
