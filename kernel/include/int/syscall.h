#pragma once

#include "libk/types.h"

#define SYSCALL_TEST 0

extern void _syscall();

static inline void call_syscall(u64 syscall) {
  asm("int $0x80" ::"a"(syscall));
}
