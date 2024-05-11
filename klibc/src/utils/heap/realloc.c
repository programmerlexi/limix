#include <math/lib.h>
#include <utils/memory/heap_wrap.h>
#include <utils/memory/memory.h>

void *realloc(void *p, size_t s, size_t o) {
  void *n = kmalloc(s);
  size_t sz = min(s, o);
  kmemcpy(n, p, sz);
  kfree(p);
  return n;
}
