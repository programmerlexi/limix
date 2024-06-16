#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define CPU_FEAT_RDRAND 1
#define CPU_FEAT_X2APIC 2
#define CPU_FEAT_APIC 4

typedef struct cpu_list_entry {
  u64 cpu_id;
  u64 capabilities;
  char vendor[12];
  struct cpu_list_entry *next;
} cpu_list_entry_t;

typedef u64 cpu_capability_t;

void cpu_init();
bool cpu_has(cpu_capability_t capability);
