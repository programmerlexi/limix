#include "kernel/task/sched/local.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/task/proc/proc.h"
#include "kernel/task/sched/common.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/thread/thread.h"
#include "libk/ipc/spinlock.h"
#include <stdbool.h>
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "local_sched"

static u32 _sched_initialize;

LocalScheduler *sched_local_init(u64 cpu) {
  spinlock(&_sched_initialize);
  log(LOGLEVEL_ANALYZE, "Creating local scheduler");
  LocalScheduler *s = kmalloc(sizeof(LocalScheduler));
  if (!s)
    kernel_panic_error("Out of memory");
  s->cpu = cpu;
  s->from_core = true;
  sched_register_cpu(s);
  log(LOGLEVEL_DEBUG, "Local scheduler initialized");
  spinunlock(&_sched_initialize);
  return s;
}

void sched_local_tick(LocalScheduler *ls) {
  spinlock(&ls->shed_lock);
  if (!ls->frames.start) {
    if (!ls->from_core) {
      kernel_panic_error("Something went horribly wrong");
    } else {
      spinunlock(&ls->shed_lock);
      return;
    }
  }
  Thread *org, *next;
  if (ls->from_core) {
    log(LOGLEVEL_ANALYZE, "Performing switch from kernel");
    ls->from_core = false;
    proc_switch(ls->core_process, ls->frames.start->frame->proc);
    org = ls->frames.start->frame->thread;
    next = ls->core_process->threads;
    spinunlock(&ls->shed_lock);
    thread_switch(org, next);
    return;
  }
  if (ls->frames.start->next) {
    debug("Switching process");
    proc_switch(ls->frames.start->frame->proc,
                ls->frames.start->next->frame->proc);
    org = ls->frames.start->frame->thread;
    if (!ls->frames.start->next->frame)
      kernel_panic_error("There seem to be a slight problem");
    next = ls->frames.start->next->frame->thread;
    ls->frames.start->frame->assigned = false;
    FrameContainer *old_frame = ls->frames.start;
    ls->frames.start = ls->frames.start->next;
    if (!ls->frames.start)
      ls->frames.end = NULL;
    kfree(old_frame);
  } else {
    log(LOGLEVEL_ANALYZE, "Switching to kernel");
    ls->from_core = true;
    proc_switch(ls->frames.start->frame->proc, ls->core_process);
    org = ls->frames.start->frame->thread;
    next = ls->core_process->threads;
    ls->frames.start = NULL;
    ls->frames.end = NULL;
  }
  spinunlock(&ls->shed_lock);

  thread_switch(next, org);
}
