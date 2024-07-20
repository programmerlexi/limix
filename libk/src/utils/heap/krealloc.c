#include "libk/math/lib.h"
#include "libk/utils/memory/heap_wrap.h"
#include "libk/utils/memory/memory.h"

void *krealloc(void *p, usz s, usz o) {
  if (s == o)
    return p;
  void *n = kmalloc(s);
  usz sz = min(s, o);
  kmemcpy(n, p, sz);
  kfree(p);
  return n;
}
