#pragma once

#include "kernel/task/proc/proc.h"
#include "kernel/task/sched/common.h"

typedef struct frame_container {
  sched_frame_t *frame;
  struct frame_container *next;
} frame_container_t;

typedef struct local_scheduler {
  frame_container_t *frames;

  process_t *core_process;
  bool from_core;

  u64 cpu;
  u32 shed_lock;

  struct local_scheduler *next;
} local_scheduler_t;

local_scheduler_t *sched_local_init(u64 cpu_id);
void sched_local_tick(local_scheduler_t *shed);
