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

extern void exec_switch(thread_t *prev, thread_t *next);
extern void exec_first_switch(thread_t *prev, thread_t *next);
extern void hcf();

thread_t *threads;
thread_t *current;

uint64_t count;
uint64_t waiting;

void async_init() {
  debug("Allocating thread list for " xstr(THREAD_LIMIT) " threads");
  threads = request_page();
  nullsafe_error(threads, "Thread init failed");
  debug("Setting up main task");
  threads[0].state = RUNNING;
  current = threads;
  threads[0].next = NULL;
  count = 1;
  waiting = 0;
  info("Finished initialization");
}

void _fire(thread_t *t) {
  thread_t *p = current;
  current = t;
  if (t != NULL) {
    if (t->state == INIT) {
      exec_first_switch(p, current);
    } else {
      exec_switch(p, current);
    }
  }
}

void _async_wrapper(result_t (*func)(variety_t), variety_t arg) {
  count++;
  current->state = RUNNING;
  current->res = func(arg);
  current->state = DONE;
  count--;
  while (true)
    _fire(threads); // Invoke first thread
}

future_t async(result_t (*func)(variety_t), variety_t arg) {
  thread_t *t = NULL;
  for (int i = 0; i < THREAD_LIMIT; i++) {
    if (threads[i].state == NONE) {
      t = &threads[i];
      break;
    }
  }
  if (t == NULL) {
    serial_writes("[!!] Failed to create thread: TOO_MANY_THREADS\n\r");
    hcf();
  }
  t->state = INIT;
  t->page = (void *)request_page();

  if (t->page == NULL) {
    serial_writes("[!!] Failed to create thread: OUT_OF_MEMORY\n\r");
    hcf();
  }

  t->sp = (void *)((uintptr_t)t->page + 0x1000 - 72);
  *(uint64_t *)(t->page + 0x1000 - 8) = (uintptr_t)_async_wrapper;
  *(uint64_t *)(t->page + 0x1000 - 16) = (uintptr_t)t->page + 0x1000;
  *(uint64_t *)(t->page + 0x1000 - 24) = (uintptr_t)func;
  *(variety_t *)(t->page + 0x1000 - 32) = arg;
  future_t future;
  future.run = t;
  future.own = current;
  return future;
}

result_t await(future_t *f) {
  waiting++;
  f->own->state = WAIT;
  f->run->next = f->own;
  while (f->run->state != DONE)
    fire(f);
  f->own->state = RUNNING;
  waiting--;
  result_t res = f->run->res;
  free_page(f->run->page);
  f->run->state = NONE;
  return res;
}

void fire(future_t *f) { _fire(f->run); }
