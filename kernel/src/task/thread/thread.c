#include "kernel/task/thread/thread.h"
#include "kernel/mm/heap.h"

static u64 _latest_tid;

Thread *thread_create() {
  Thread *t = (Thread *)kzalloc(sizeof(Thread));
  t->tid = ++_latest_tid;
  t->state = THREAD_INIT;
  return t;
}

void thread_destroy(Thread *t) { kfree(t); }

extern void exec_thread_switch(u64 *prev, u64 *next);

void thread_switch(Thread *t, Thread *p) {
  if (p->state == THREAD_RUNNING || p->state == THREAD_INIT)
    p->state = THREAD_IDLE;
  t->state = THREAD_RUNNING;
  exec_thread_switch(&(p->rsp), &(t->rsp));
}
