#pragma once

#include "libk/types.h"

typedef struct heapseg {
  usz size;
  struct heapseg *prev;
  struct heapseg *next;
  u64 used;
} heapseg_t;

void heap_init();
u32 heap_get_used();

void *kmalloc(usz count);
void kfree(void *pointer);
