#pragma once

#include "kernel/task/proc/proc.h"
#include "kernel/task/thread/thread.h"
#include <stdbool.h>

#define TASK_STACK_ADDRESS 0x7ffff000

typedef struct sched_frame {
  process_t *proc;
  thread_t *thread;

  bool assigned;

  struct sched_frame *next;
} sched_frame_t;

typedef struct frame_queue {
  sched_frame_t *start;
  sched_frame_t *end;
} frame_queue_t;

void sched_create(void(*start), i64 cpu, u64 arg);
