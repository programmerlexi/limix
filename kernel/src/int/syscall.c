#include "kernel/int/syscall.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/int/idt.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(int_frame_t *f) {
  u64 cpu = (u64)get_processor();
  switch (f->rax) {
  case SYSCALL_TEST:
    log(LOGLEVEL_INFO, "Syscall test");
    logf(LOGLEVEL_INFO, "CPU: %u", cpu);
    break;
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
