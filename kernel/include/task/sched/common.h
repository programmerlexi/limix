#pragma once

#include "kernel/task/proc/proc.h"
#include "kernel/task/thread/thread.h"
#include <stdbool.h>

#define TASK_STACK_ADDRESS 0x7ffff000

typedef struct SchedulerFrameStruct {
  Process *proc;
  Thread *thread;

  bool assigned;

  struct SchedulerFrameStruct *next;
} SchedulerFrame;

typedef struct FrameQueueStruct {
  SchedulerFrame *start;
  SchedulerFrame *end;
} FrameQueue;

void sched_create(void(*start), i64 cpu, u64 arg);
