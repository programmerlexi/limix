#pragma once

#include <stdbool.h>
#include <utils/results.h>
#include <utils/variety.h>

#define THREAD_LIMIT 128

enum thread_state { NONE, RUNNING, INIT, WAIT, DONE };

typedef struct thread {
  void *sp;
  void *page;
  enum thread_state state;
  union {
    struct thread *next;
    result_t res;
  };
} thread_t;

typedef struct {
  thread_t *run;
  thread_t *own;
} future_t;

void fire(future_t *);
result_t await(future_t *);
future_t async(result_t (*func)(variety_t), variety_t arg);

void async_init();
