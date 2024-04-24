#pragma once

#include <task/sched/common.h>

typedef struct frame_container {
  sched_frame_t *frame;
  struct frame_container *next;
} frame_container_t;

typedef struct local_scheduler {
  frame_container_t *frames;

  uint64_t cpu;
  uint32_t shed_lock;

  struct local_scheduler *next;
} local_scheduler_t;

local_scheduler_t *sched_local_init(uint64_t cpu_id);
void sched_local_tick(local_scheduler_t *shed);
