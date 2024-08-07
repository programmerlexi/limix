#pragma once

#include "libk/types.h"
#include "libk/utils/strings/xstr.h"

enum ThreadStateEnum {
  THREAD_IDLE,
  THREAD_RUNNING,
  THREAD_BLOCKED,
  THREAD_INIT,
  THREAD_TERMINATE
};

typedef struct ThreadStruct {
  u64 tid;
  enum ThreadStateEnum state;
  u64 rsp;
  u64 entry;
  xstr_t cwd;

  struct ThreadStruct *next;
} Thread;

void thread_switch(Thread *next, Thread *prev);

Thread *thread_create();
void thread_destroy(Thread *thread);
