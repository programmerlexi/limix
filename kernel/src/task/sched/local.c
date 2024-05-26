#include "kernel/task/sched/local.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
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
  sched_register_cpu(s);
  log(LOGLEVEL_INFO, "Local scheduler initialized");
  spinunlock(&_sched_initialize);
  return s;
}

void sched_local_tick(local_scheduler_t *ls) {
  spinlock(&ls->shed_lock);
  debug("Ticking local");
  if (!ls->frames) {
    spinunlock(&ls->shed_lock);
    return;
  }
  thread_t *org, *next;
  if (ls->frames->next) {
    debug("Switching process");
    proc_switch(ls->frames->frame->proc, ls->frames->next->frame->proc);
    org = ls->frames->frame->thread;
    next = ls->frames->next->frame->thread;
    ls->frames->frame->assigned = false;
    ls->frames = ls->frames->next;
  } else {
    org = ls->frames->frame->thread;
    next = org;
  }
  spinunlock(&ls->shed_lock);

  debug("Switching thread");
  thread_switch(next, org);
}
