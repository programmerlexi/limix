#include "defines.h"
#include "math/lib.h"
#include "utils/strings/strings.h"

BOOL kstrncmp(char *a, char *b, usz n) {
  if (min(kstrlen(a), n) != min(kstrlen(b), n))
    return FALSE;
  i32 l = min(min(kstrlen(a), kstrlen(b)), n);
  for (i32 i = 0; i < l; i++) {
    if (a[i] != b[i])
      return FALSE;
  }
  return TRUE;
}
