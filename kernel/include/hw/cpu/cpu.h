#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define CPU_FEAT_RDRAND 1
#define CPU_FEAT_X2APIC 2
#define CPU_FEAT_APIC 4

typedef struct CpuListEntryStruct {
  u64 cpu_id;
  u64 capabilities;
  char vendor[13];
  struct CpuListEntryStruct *next;
} CpuListEntry;

typedef u64 CpuCapability;

void cpu_init();
bool cpu_has(CpuCapability capability);
char *cpu_vendor();

i64 cpu_get_random_cpu();
