#include <mm/heap.h>
#include <stdint.h>
#include <task/thread/thread.h>

static u64 _latest_tid = 0;

thread_t *thread_create() {
  thread_t *t = (thread_t *)malloc(sizeof(thread_t));
  t->tid = ++_latest_tid;
  t->state = THREAD_INIT;
  return t;
}

void thread_destroy(thread_t *t) { free(t); }

extern void exec_thread_switch(u64 *prev, u64 *next);

void thread_switch(thread_t *t, thread_t *p) {
  p->state = THREAD_IDLE;
  t->state = THREAD_RUNNING;
  exec_thread_switch(&p->rsp, &t->rsp);
}
