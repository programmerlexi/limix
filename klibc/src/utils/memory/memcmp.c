#include <stddef.h>
#include <stdint.h>
#include <types.h>

i32 memcmp(const void *s1, const void *s2, size_t n) {
  if (!n) {
    return 0;
  } // nothing to compare?
  const u8 *p1 = (const u8 *)s1;
  const u8 *p2 = (const u8 *)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }
  return 0;
}
