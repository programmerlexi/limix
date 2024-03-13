#include <io/serial/serial.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <stdint.h>
#include <task/async.h>
#include <utils/memory/memory.h>
#include <utils/results.h>
#include <utils/variety.h>

extern void exec_switch(thread_t *prev, thread_t *next);
extern void hcf();

thread_t *threads;
thread_t *current;

uint64_t count;
uint64_t waiting;

void async_init() {
  threads = request_page();
  if (threads == NULL) {
    serial_writes("[!!] Couldn't initialize threads!\n\r");
    hcf();
  }
  memset(threads, 0, 0x1000);
  threads[0].state = RUNNING;
  current = threads;
  count = 1;
  waiting = 0;
}

void _fire(thread_t *t) { exec_switch(current, t); }

void _async_wrapper(result_t (*func)(variety_t), variety_t arg) {
  current->res = func(arg);
  current->state = DONE;
  while (true)
    _fire(current->next);
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
  t->state = RUNNING;
  t->page = (void *)HHDM(request_page());
  t->sp = (void *)((uintptr_t)t->page + 0x1000 - 80);
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
  f->own->state = WAIT;
  while (f->run->state != DONE)
    fire(f);
  result_t res = f->run->res;
  free_page(f->run->page);
  f->run->state = NONE;
  return res;
}

void fire(future_t *f) { exec_switch(f->own, f->run); }
