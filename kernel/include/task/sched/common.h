#pragma once

#include "task/proc/proc.h"
#include "task/thread/thread.h"
#include <stdbool.h>

typedef struct sched_frame {
  process_t *proc;
  thread_t *thread;

  bool assigned;

  struct sched_frame *next;
} sched_frame_t;
