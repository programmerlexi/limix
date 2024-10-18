#include <libk/types.h>
#include <libk/utils/memory/memory.h>

void *kmemmove(void *dest, const void *src, usz n) {
  u8 *pdest = (u8 *)dest;
  const u8 *psrc = (const u8 *)src;

  if (src > dest) {
    kmemcpy(dest, src, n);
  } else if (src < dest) {
    for (usz i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }
  return dest;
}
