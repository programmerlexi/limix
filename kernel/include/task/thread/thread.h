#pragma once

#include <stdint.h>
#include <utils/strings/xstr.h>

enum thread_state {
  THREAD_IDLE,
  THREAD_RUNNING,
  THREAD_BLOCKED,
  THREAD_INIT,
  THREAD_TERMINATE
};

typedef struct thread {
  uint64_t tid;
  enum thread_state state;
  uint64_t rsp;
  uint64_t entry;
  xstr_t cwd;

  struct thread *next;
} thread_t;

void thread_switch(thread_t *, thread_t *);

thread_t *thread_create();
void thread_destroy(thread_t *);
