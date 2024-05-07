#include <math/lib.h>
#include <utils/memory/heap_wrap.h>
#include <utils/memory/memory.h>

void *clone(void *p, size_t n) {
  void *np = malloc(ALIGN_UP(n, 16));
  memset(np, 0, ALIGN_UP(n, 16));
  memcpy(np, p, n);
  return np;
}
