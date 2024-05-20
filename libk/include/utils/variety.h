#pragma once

#include "libk/types.h"

typedef union {
  u8 value_u8;
  u16 value_u16;
  u32 value_u32;
  u64 value_u64;
  uptr value_uptr;
  void *ptr;
  i8 value_i8;
  i16 value_i16;
  i32 value_i32;
  i64 value_i64;
} variety_t;
