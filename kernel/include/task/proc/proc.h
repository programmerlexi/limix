#pragma once

#include <stdint.h>
#include <task/thread/thread.h>

typedef struct process {
  uint64_t pid;

  uint64_t uid;
  uint64_t gid;
  uint64_t euid;
  uint64_t egid;

  uint64_t thread_count;
  thread_t *threads;

  uint64_t cpu;

  uint64_t cr4;

  xstr_t name;

  struct process *next;
} process_t;

process_t *proc_create();
void proc_destroy(process_t *);

void proc_switch(process_t *, process_t *);
