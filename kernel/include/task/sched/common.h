#pragma once

#include "task/proc/proc.h"
#include "task/thread/thread.h"

typedef struct sched_frame {
  process_t *proc;
  thread_t *thread;

  BOOL assigned;

  struct sched_frame *next;
} sched_frame_t;
