#include <stddef.h>
#include <stdint.h>
#include <types.h>

void *memset(void *dst, i32 sval, size_t count) {
  if (!count) {
    return dst;
  } // nothing to set?
  u64 addr = (u64)dst;
  u64 val = (sval & 0xFF); // create a 64-bit version of 'sval'
  val |= ((val << 8) & 0xFF00);
  val |= ((val << 16) & 0xFFFF0000);
  val |= ((val << 32) & 0xFFFFFFFF00000000);

  while (count >= 8) {
    *(u64 *)addr = (u64)val;
    addr += 8;
    count -= 8;
  }
  while (count >= 4) {
    *(u32 *)addr = (u32)val;
    addr += 4;
    count -= 4;
  }
  while (count >= 2) {
    *(u16 *)addr = (u16)val;
    addr += 2;
    count -= 2;
  }
  while (count >= 1) {
    *(u8 *)addr = (u8)val;
    addr += 1;
    count -= 1;
  }
  return dst;
}
