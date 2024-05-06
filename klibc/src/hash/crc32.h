#include <hash/crc32.h>
#include <stdint.h>

static uint32_t crc32_poly(uint8_t i) {
  uint32_t r = (uint32_t)i;
  for (int j = 0; j < 8; j++) {
    bool l = r & 1;
    r >>= 1;
    if (l)
      r ^= 0xEDB88320;
  }
  return r;
}

uint32_t crc32(uint8_t *p, uint64_t count) {
  uint32_t r = 0xffffffff;
  while (count-- != 0)
    r = crc32_poly(((uint8_t)r ^ *(p++))) ^ (r >> 8);
  return ~r;
}
