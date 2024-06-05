#include "kernel/int/syscall.h"
#include "kernel/debug.h"
#include "kernel/int/idt.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(int_frame_t *f) {
  switch (f->rax) {
  case SYSCALL_TEST:
    log(LOGLEVEL_INFO, "Syscall test");
    break;
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
