#include <stddef.h>
#include <stdint.h>

void *memset(void *dst, int sval, size_t count) {
  if (!count) {
    return dst;
  } // nothing to set?
  uint64_t addr = (uint64_t)dst;
  uint64_t val = (sval & 0xFF); // create a 64-bit version of 'sval'
  val |= ((val << 8) & 0xFF00);
  val |= ((val << 16) & 0xFFFF0000);
  val |= ((val << 32) & 0xFFFFFFFF00000000);

  while (count >= 8) {
    *(uint64_t *)addr = (uint64_t)val;
    addr += 8;
    count -= 8;
  }
  while (count >= 4) {
    *(uint32_t *)addr = (uint32_t)val;
    addr += 4;
    count -= 4;
  }
  while (count >= 2) {
    *(uint16_t *)addr = (uint16_t)val;
    addr += 2;
    count -= 2;
  }
  while (count >= 1) {
    *(uint8_t *)addr = (uint8_t)val;
    addr += 1;
    count -= 1;
  }
  return dst;
}
