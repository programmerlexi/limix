#pragma once

#include "kernel/task/proc/proc.h"
#include "kernel/task/sched/common.h"

typedef struct FrameContainerStruct {
  SchedulerFrame *frame;
  struct FrameContainerStruct *next;
} FrameContainer;

typedef struct FrameContainerQueueStruct {
  FrameContainer *start;
  FrameContainer *end;
} FrameContainerQueue;

typedef struct LocalSchedulerStruct {
  FrameContainerQueue frames;

  Process *core_process;
  bool from_core;

  u64 cpu;
  u32 shed_lock;

  struct LocalSchedulerStruct *next;
} LocalScheduler;

LocalScheduler *sched_local_init(u64 cpu_id);
void sched_local_tick(LocalScheduler *shed);
