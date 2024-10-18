#include <kernel/mm/mm.h>
#include <libk/types.h>
#include <stdbool.h>
#include <stddef.h>

uptr mm_fix(uptr ptr) {
  if (ptr & 0x0000800000000000)
    return ptr | 0xffff000000000000;
  else
    return ptr & 0x0000ffffffffffff;
}
bool mm_is_physical(uptr ptr) { return ptr <= 0xffff800000000000; }
bool mm_is_virtual(uptr ptr) { return !mm_is_physical(ptr); }
