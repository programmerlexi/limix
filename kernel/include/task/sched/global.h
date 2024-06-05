#pragma once

#include "kernel/task/sched/local.h"

void sched_glob_init();

void sched_glob_tick();

void sched_glob_release();
void sched_glob_aquire();

void sched_register_cpu(local_scheduler_t *shed);

void sched_glob_list_processes();

local_scheduler_t *sched_get_local(u64 cpu);
