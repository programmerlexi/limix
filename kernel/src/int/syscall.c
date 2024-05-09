#include "kernel.h"
#include <debug.h>
#include <int/syscall.h>
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(u64 syscall_id, u64 p0, u64 p1, u64 p2, u64 p3) {
  switch (syscall_id) {
  case SYSCALL_TEST:
    debug("Syscall test");
    break;
  default:
    debug("Got unknown syscall");
    break;
  }
}
