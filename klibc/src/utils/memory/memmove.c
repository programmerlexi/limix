#include "types.h"
#include "utils/memory/memory.h"
#include <stddef.h>
#include <stdint.h>

void *kmemmove(void *dest, const void *src, size_t n) {
  u8 *pdest = (u8 *)dest;
  const u8 *psrc = (const u8 *)src;

  if (src > dest) {
    kmemcpy(dest, src, n);
  } else if (src < dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }
  return dest;
}
