#include <libk/crypt/crc32.h>
#include <libk/types.h>
#include <stdbool.h>

static u32 _crc32_poly(u8 i) {
  u32 r = (u32)i;
  for (i32 j = 0; j < 8; j++) {
    bool l = r & 1;
    r >>= 1;
    if (l)
      r ^= 0xEDB88320;
  }
  return r;
}

u32 crc32(u8 *p, u64 count) {
  u32 r = 0xffffffff;
  while (count-- != 0)
    r = _crc32_poly(((u8)r ^ *(p++))) ^ (r >> 8);
  return ~r;
}
