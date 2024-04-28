#include <debug.h>
#include <int/syscall.h>
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(uint64_t syscall_id, uint64_t p0, uint64_t p1, uint64_t p2,
             uint64_t p3) {
  switch (syscall_id) {
  case SYSCALL_TEST:
    debug("Syscall test");
    break;
  default:
    debug("Got unknown syscall");
    break;
  }
}
