#pragma once

#include "libk/types.h"

#define HEAP_BASE 0xfffffff800000000

typedef struct heapseg {
  usz size;
  struct heapseg *prev;
  struct heapseg *next;
  u64 used;
} heapseg_t;

void heap_init();
u32 heap_get_used();

void *kmalloc(usz count);
void *kcalloc(usz count);
void kfree(void *pointer);
