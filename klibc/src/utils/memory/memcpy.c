#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dst, const void *src, size_t count) {
  if (!count) {
    return dst;
  } // nothing to copy?
  uint64_t dstaddr = (uint64_t)dst;
  uint64_t srcaddr = (uint64_t)src;

  while (count >= 8) {
    *(uint64_t *)dstaddr = *(uint64_t *)srcaddr;
    dstaddr += 8;
    srcaddr += 8;
    count -= 8;
  }
  while (count >= 4) {
    *(uint32_t *)dstaddr = *(uint32_t *)srcaddr;
    dstaddr += 4;
    srcaddr += 4;
    count -= 4;
  }
  while (count >= 2) {
    *(uint16_t *)dstaddr = *(uint16_t *)srcaddr;
    dstaddr += 2;
    srcaddr += 2;
    count -= 2;
  }
  while (count >= 1) {
    *(uint8_t *)dstaddr = *(uint8_t *)srcaddr;
    dstaddr += 1;
    srcaddr += 1;
    count -= 1;
  }
  return dst;
}
