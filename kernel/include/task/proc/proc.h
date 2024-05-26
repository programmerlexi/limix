#pragma once

#include "kernel/task/thread/thread.h"
#include "libk/types.h"

typedef struct process {
  u64 pid;

  u64 uid;
  u64 gid;
  u64 euid;
  u64 egid;

  u64 thread_count;
  thread_t *threads;

  u64 cpu;

  u64 cr3;

  xstr_t name;

  struct process *next;
} process_t;

process_t *proc_create();
void proc_destroy(process_t *process);

void proc_switch(process_t *prev, process_t *next);
