#include "math/lib.h"
#include "utils/memory/heap_wrap.h"
#include "utils/memory/memory.h"

void *realloc(void *p, usz s, usz o) {
  void *n = kmalloc(s);
  usz sz = min(s, o);
  kmemcpy(n, p, sz);
  kfree(p);
  return n;
}
