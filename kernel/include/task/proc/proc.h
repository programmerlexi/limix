#pragma once

#include "kernel/task/thread/thread.h"
#include "libk/types.h"

typedef struct ProcessStruct {
  u64 pid;

  u64 uid;
  u64 gid;
  u64 euid;
  u64 egid;

  u64 thread_count;
  Thread *threads;

  u64 cpu;

  u64 cr3;

  xstr_t name;

  struct ProcessStruct *prev;
  struct ProcessStruct *next;
} Process;

Process *proc_create();
void proc_destroy(Process *process);

void proc_switch(Process *prev, Process *next);
