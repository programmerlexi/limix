#include "kernel/task/thread/thread.h"
#include "kernel/mm/heap.h"

static u64 _latest_tid;

thread_t *thread_create() {
  thread_t *t = (thread_t *)kcalloc(sizeof(thread_t));
  t->tid = ++_latest_tid;
  t->state = THREAD_INIT;
  return t;
}

void thread_destroy(thread_t *t) { kfree(t); }

extern void exec_thread_switch(u64 *prev, u64 *next);

void thread_switch(thread_t *t, thread_t *p) {
  if (p->state == THREAD_RUNNING || p->state == THREAD_INIT)
    p->state = THREAD_IDLE;
  t->state = THREAD_RUNNING;
  exec_thread_switch(&(p->rsp), &(t->rsp));
}
