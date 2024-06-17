#include "kernel/int/syscall.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/int/idt.h"
#include "kernel/panic.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"
#include "libk/ipc/spinlock.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(int_frame_t *f) {
  u64 cpu = (u64)get_processor();
  local_scheduler_t *ls = sched_get_local(cpu);
  switch (f->rax) {
  case SYSCALL_TEST:
    static u32 lck = 0;
    spinlock(&lck);
    log(LOGLEVEL_INFO, "Syscall test");
    logf(LOGLEVEL_INFO, "CPU: %u", cpu);
    if (ls) {
      log(LOGLEVEL_INFO, "Local scheduler is initialized");
      if (ls->frames.start) {
        logf(LOGLEVEL_INFO, "PID: %u TID: %u",
             ls->frames.start->frame->proc->pid,
             ls->frames.start->frame->thread->tid);
        logf(LOGLEVEL_INFO, "Process name: %s",
             ls->frames.start->frame->proc->name.cstr);
      }
    }
    spinunlock(&lck);
    break;
  case SYSCALL_YIELD:
    if (!ls) {
      log(LOGLEVEL_FATAL, "Received yield from non-existent cpu");
      kpanic();
    }
    sched_local_tick(ls);
    break;
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
