#include "kernel/task/sched/local.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/task/proc/proc.h"
#include "kernel/task/sched/common.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/thread/thread.h"
#include "libk/ipc/spinlock.h"
#include <stdbool.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "local_sched"

static u32 _sched_initialize = 1;

void unlock_lschedi() { spinunlock(&_sched_initialize); }

local_scheduler_t *sched_local_init(u64 cpu) {
  spinlock(&_sched_initialize);
  log(LOGLEVEL_ANALYZE, "Creating local scheduler");
  local_scheduler_t *s = kmalloc(sizeof(local_scheduler_t));
  s->cpu = cpu;
  s->from_core = true;
  sched_register_cpu(s);
  log(LOGLEVEL_INFO, "Local scheduler initialized");
  spinunlock(&_sched_initialize);
  return s;
}

void sched_local_tick(local_scheduler_t *ls) {
  spinlock(&ls->shed_lock);
  debug("Ticking local");
  if (!ls->frames) {
    if (!ls->from_core) {
      kernel_panic_error("Something went horribly wrong");
    } else {
      spinunlock(&ls->shed_lock);
      return;
    }
  }
  if (ls->from_core) {
    log(LOGLEVEL_ANALYZE, "Performing switch from kernel");
    ls->from_core = false;
    proc_switch(ls->core_process, ls->frames->frame->proc);
    spinunlock(&ls->shed_lock);
    thread_switch(ls->frames->frame->thread, ls->core_process->threads);
    return;
  }
  thread_t *org, *next;
  if (ls->frames->next) {
    debug("Switching process");
    proc_switch(ls->frames->frame->proc, ls->frames->next->frame->proc);
    org = ls->frames->frame->thread;
    if (!ls->frames->next->frame)
      kernel_panic_error("There seem to be a slight problem");
    next = ls->frames->next->frame->thread;
    ls->frames->frame->assigned = false;
    frame_container_t *old_frame = ls->frames;
    ls->frames = ls->frames->next;
    kfree(old_frame);
  } else {
    log(LOGLEVEL_ANALYZE, "Switching to kernel");
    ls->from_core = true;
    proc_switch(ls->frames->frame->proc, ls->core_process);
    org = ls->frames->frame->thread;
    next = ls->core_process->threads;
  }
  spinunlock(&ls->shed_lock);

  debug("Switching thread");
  thread_switch(next, org);
}
