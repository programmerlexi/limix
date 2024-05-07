#include <utils/memory/heap_wrap.h>
#include <utils/memory/memory.h>

void *clone(void *p, size_t n) { return memcpy(malloc(n), p, n); }
