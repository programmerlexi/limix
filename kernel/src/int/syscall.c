#include "kernel/int/syscall.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/int/idt.h"
#include "kernel/panic.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(int_frame_t *f) {
  u64 cpu = (u64)get_processor();
  switch (f->rax) {
  case SYSCALL_TEST:
    log(LOGLEVEL_INFO, "Syscall test");
    logf(LOGLEVEL_INFO, "CPU: %u", cpu);
    break;
  case SYSCALL_YIELD:
    local_scheduler_t *ls = sched_get_local(cpu);
    if (!ls)
      kpanic();
    sched_local_tick(ls);
    break;
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
