#pragma once

#include <stdbool.h>
#include <utils/results.h>
#include <utils/variety.h>

#define THREAD_LIMIT 128

enum async_state {
  ASYNC_NONE,
  ASYNC_RUNNING,
  ASYNC_INIT,
  ASYNC_WAIT,
  ASYNC_DONE
};

typedef struct task {
  void *sp;
  void *page;
  enum async_state state;
  union {
    struct task *next;
    result_t res;
  };
} task_t;

typedef struct {
  task_t *run;
  task_t *own;
} future_t;

void fire(future_t *);
result_t await(future_t *);
future_t async(result_t (*func)(variety_t), variety_t arg);

void async_init();
