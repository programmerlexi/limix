#define DEBUG_MODULE "async"

#include <debug.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <mm/heap.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <stdint.h>
#include <task/async.h>
#include <utils/memory/memory.h>
#include <utils/memory/safety.h>
#include <utils/results.h>
#include <utils/variety.h>

extern void exec_switch(task_t *prev, task_t *next);
extern void exec_first_switch(task_t *prev, task_t *next);
extern void hcf();

static task_t *threads;
static task_t *current;

static u64 count;
static u64 waiting;

void async_init() {
  log(LOGLEVEL_ANALYZE,
      "Allocating thread list for " xstr(THREAD_LIMIT) " threads");
  threads = request_page();
  nullsafe_error(threads, "Thread init failed");
  log(LOGLEVEL_DEBUG, "Setting up main task");
  threads[0].state = ASYNC_RUNNING;
  current = threads;
  threads[0].next = NULL;
  count = 1;
  waiting = 0;
  log(LOGLEVEL_INFO, "Finished initialization");
}

static void _fire(task_t *t) {
  task_t *p = current;
  current = t;
  if (t != NULL) {
    if (t->state == ASYNC_INIT) {
      exec_first_switch(p, current);
    } else {
      exec_switch(p, current);
    }
  }
}

static void _async_wrapper(result_t (*func)(variety_t), variety_t arg) {
  count++;
  current->state = ASYNC_RUNNING;
  current->res = func(arg);
  current->state = ASYNC_DONE;
  count--;
  while (true)
    _fire(threads); // Invoke first thread
}

future_t async(result_t (*func)(variety_t), variety_t arg) {
  task_t *t = NULL;
  for (int i = 0; i < THREAD_LIMIT; i++) {
    if (threads[i].state == ASYNC_NONE) {
      t = &threads[i];
      break;
    }
  }
  if (t == NULL) {
    serial_writes("[!!] Failed to create thread: TOO_MANY_THREADS\n\r");
    hcf();
  }
  t->state = ASYNC_INIT;
  t->page = (void *)request_page();

  if (t->page == NULL) {
    serial_writes("[!!] Failed to create thread: OUT_OF_MEMORY\n\r");
    hcf();
  }

  t->sp = (void *)((uintptr_t)t->page + 0x1000 - 72);
  *(u64 *)(t->page + 0x1000 - 8) = (uintptr_t)_async_wrapper;
  *(u64 *)(t->page + 0x1000 - 16) = (uintptr_t)t->page + 0x1000;
  *(u64 *)(t->page + 0x1000 - 24) = (uintptr_t)func;
  *(variety_t *)(t->page + 0x1000 - 32) = arg;
  future_t future;
  future.run = t;
  future.own = current;
  return future;
}

result_t await(future_t *f) {
  waiting++;
  f->own->state = ASYNC_WAIT;
  f->run->next = f->own;
  while (f->run->state != ASYNC_DONE)
    fire(f);
  f->own->state = ASYNC_RUNNING;
  waiting--;
  result_t res = f->run->res;
  free_page(f->run->page);
  f->run->state = ASYNC_NONE;
  return res;
}

void fire(future_t *f) { _fire(f->run); }
