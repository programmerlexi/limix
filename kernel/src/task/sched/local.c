#define DEBUG_MODULE "local_sched"

#include <debug.h>
#include <kipc/spinlock.h>
#include <mm/heap.h>
#include <stdint.h>
#include <task/sched/common.h>
#include <task/sched/global.h>
#include <task/sched/local.h>
#include <task/thread/thread.h>

static uint32_t sched_initialize = 1;

void unlock_lschedi() { spinunlock(&sched_initialize); }

local_scheduler_t *sched_local_init(uint64_t cpu) {
  spinlock(&sched_initialize);
  log(LOGLEVEL_ANALYZE, "Creating local scheduler");
  local_scheduler_t *s = malloc(sizeof(local_scheduler_t));
  s->cpu = cpu;
  sched_register_cpu(s);
  log(LOGLEVEL_INFO, "Local scheduler initialized");
  spinunlock(&sched_initialize);
  return s;
}

void sched_local_tick(local_scheduler_t *ls) {
  spinlock(&ls->shed_lock);
  if (!ls->frames) {
    spinunlock(&ls->shed_lock);
    return;
  }
  proc_switch(ls->frames->frame->proc, ls->frames->next->frame->proc);
  thread_t *org = ls->frames->frame->thread;
  thread_t *next = ls->frames->next->frame->thread;
  ls->frames->frame->assigned = false;
  ls->frames = ls->frames->next;
  spinunlock(&ls->shed_lock);

  thread_switch(org, next);
}
