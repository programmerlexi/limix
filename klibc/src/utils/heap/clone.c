#include "math/lib.h"
#include "utils/memory/heap_wrap.h"
#include "utils/memory/memory.h"

void *clone(void *p, size_t n) {
  void *np = kmalloc(ALIGN_UP(n, 16));
  kmemset(np, 0, ALIGN_UP(n, 16));
  kmemcpy(np, p, n);
  return np;
}
