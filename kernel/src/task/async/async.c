#include "task/async.h"
#include "config.h"
#include "debug.h"
#include "defines.h"
#include "mm/mm.h"
#include "utils/memory/safety.h"
#include "utils/results.h"
#include "utils/variety.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "async"

extern void exec_switch(task_t *prev, task_t *next);
extern void exec_first_switch(task_t *prev, task_t *next);
extern void hcf();

static task_t *_threads;
static task_t *_current;

static u64 _count;
static u64 _waiting;

void async_init() {
  log(LOGLEVEL_ANALYZE, "Allocating task list for " xstr(TASK_LIMIT) " tasks");
  _threads = request_page();
  nullsafe_error(_threads, "Task init failed");
  log(LOGLEVEL_DEBUG, "Setting up main task");
  _threads[0].state = ASYNC_RUNNING;
  _current = _threads;
  _threads[0].next = NULL;
  _count = 1;
  _waiting = 0;
  log(LOGLEVEL_INFO, "Finished initialization");
}

static void _fire(task_t *t) {
  task_t *p = _current;
  _current = t;
  if (t != NULL) {
    if (t->state == ASYNC_INIT) {
      exec_first_switch(p, _current);
    } else {
      exec_switch(p, _current);
    }
  }
}

static void _async_wrapper(result_t (*func)(variety_t), variety_t arg) {
  _count++;
  _current->state = ASYNC_RUNNING;
  _current->res = func(arg);
  _current->state = ASYNC_DONE;
  _count--;
  while (TRUE)
    _fire(_threads); // Invoke first thread
}

future_t async(result_t (*func)(variety_t), variety_t arg) {
  task_t *t = NULL;
  for (i32 i = 0; i < TASK_LIMIT; i++)
    if (_threads[i].state == ASYNC_NONE) {
      t = &_threads[i];
      break;
    }
  nullsafe_error(t, "Failed to create task: TOO_MANY_THREADS");
  t->state = ASYNC_INIT;
  t->page = (void *)request_page();

  nullsafe_error(t->page, "Failed to create task: OUT_OF_MEMORY");

  t->sp = (void *)((uintptr_t)t->page + 0x1000 - 72);
  *(u64 *)(t->page + 0x1000 - 8) = (uintptr_t)_async_wrapper;
  *(u64 *)(t->page + 0x1000 - 16) = (uintptr_t)t->page + 0x1000;
  *(u64 *)(t->page + 0x1000 - 24) = (uintptr_t)func;
  *(variety_t *)(t->page + 0x1000 - 32) = arg;
  future_t future;
  future.run = t;
  future.own = _current;
  return future;
}

result_t await(future_t *f) {
  _waiting++;
  f->own->state = ASYNC_WAIT;
  f->run->next = f->own;
  while (f->run->state != ASYNC_DONE)
    fire(f);
  f->own->state = ASYNC_RUNNING;
  _waiting--;
  result_t res = f->run->res;
  free_page(f->run->page);
  f->run->state = ASYNC_NONE;
  return res;
}

void fire(future_t *f) { _fire(f->run); }
