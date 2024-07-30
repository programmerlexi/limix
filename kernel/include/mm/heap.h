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
usz heap_get_used();
usz heap_get_free();
usz heap_get_overhead();

void heap_print_usage();

void *kmalloc(usz count);
void *kzalloc(usz count);
void *kcalloc(usz count, usz value);
void kfree(void *pointer);
