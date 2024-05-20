#include "libk/math/lib.h"
#include "libk/utils/memory/heap_wrap.h"
#include "libk/utils/memory/memory.h"

void *clone(void *p, usz n) {
  void *np = kmalloc(ALIGN_UP(n, 16));
  kmemset(np, 0, ALIGN_UP(n, 16));
  kmemcpy(np, p, n);
  return np;
}
