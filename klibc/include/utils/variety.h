#pragma once

#include <stdint.h>

typedef union {
  uint8_t u8;
  uint16_t u16;
  uint32_t u32;
  uint64_t u64;
  uintptr_t uptr;
  void *ptr;
  int8_t i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;
} variety_t;
