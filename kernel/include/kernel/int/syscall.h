#pragma once

#include <libk/types.h>

extern void _syscall();

static inline void call_syscall(u64 syscall) {
  asm("int $0x80" ::"a"(syscall));
}
