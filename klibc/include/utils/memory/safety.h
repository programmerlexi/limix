#pragma once

#define nullsafe_with(pointer, block)                                          \
  if (!pointer)                                                                \
  block
#define nullsafe(pointer) nullsafe_with(pointer, return)
#define nullsafe_error(pointer, error)                                         \
  nullsafe_with(pointer, { kernel_panic_error(error); })
