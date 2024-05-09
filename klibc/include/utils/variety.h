#pragma once

#include <stdint.h>

typedef union {
  uint8_t value_u8;
  uint16_t value_u16;
  uint32_t value_u32;
  uint64_t value_u64;
  uintptr_t value_uptr;
  void *ptr;
  int8_t value_i8;
  int16_t value_i16;
  int32_t value_i32;
  int64_t value_i64;
} variety_t;
