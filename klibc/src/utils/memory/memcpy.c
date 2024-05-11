#include "types.h"
#include <stddef.h>
#include <stdint.h>

void *kmemcpy(void *dst, const void *src, size_t count) {
  if (!count) {
    return dst;
  } // nothing to copy?
  u64 dstaddr = (u64)dst;
  u64 srcaddr = (u64)src;

  while (count >= 8) {
    *(u64 *)dstaddr = *(u64 *)srcaddr;
    dstaddr += 8;
    srcaddr += 8;
    count -= 8;
  }
  while (count >= 4) {
    *(u32 *)dstaddr = *(u32 *)srcaddr;
    dstaddr += 4;
    srcaddr += 4;
    count -= 4;
  }
  while (count >= 2) {
    *(u16 *)dstaddr = *(u16 *)srcaddr;
    dstaddr += 2;
    srcaddr += 2;
    count -= 2;
  }
  while (count >= 1) {
    *(u8 *)dstaddr = *(u8 *)srcaddr;
    dstaddr += 1;
    srcaddr += 1;
    count -= 1;
  }
  return dst;
}
