#include "kernel/int/syscall.h"
#include "kernel/debug.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(u64 syscall_id, u64 p0, u64 p1, u64 p2, u64 p3) {
  switch (syscall_id) {
  case SYSCALL_TEST:
    log(LOGLEVEL_INFO, "Syscall test");
    break;
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
