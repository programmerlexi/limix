#pragma once

#include "libk/types.h"

#define HEAP_BASE 0xfffffff800000000
#define HEAP_CANARY0 0x6865617062697264
#define HEAP_CANARY1 0x6269726468656170

typedef struct heapseg {
  u64 canary0;
  usz size;
  struct heapseg *prev;
  struct heapseg *next;
  u64 used;
  u64 canary1;
} __attribute__((packed)) heapseg_t;

void heap_init();
u32 heap_get_used();

void *kmalloc(usz count);
void *kcalloc(usz count);
void kfree(void *pointer);
