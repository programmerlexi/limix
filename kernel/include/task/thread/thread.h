#pragma once

#include "types.h"
#include "utils/strings/xstr.h"
#include <stdint.h>

enum thread_state {
  THREAD_IDLE,
  THREAD_RUNNING,
  THREAD_BLOCKED,
  THREAD_INIT,
  THREAD_TERMINATE
};

typedef struct thread {
  u64 tid;
  enum thread_state state;
  u64 rsp;
  u64 entry;
  xstr_t cwd;

  struct thread *next;
} thread_t;

void thread_switch(thread_t *prev, thread_t *next);

thread_t *thread_create();
void thread_destroy(thread_t *thread);
