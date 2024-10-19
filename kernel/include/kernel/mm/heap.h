#pragma once

#include <libk/types.h>

#define HEAP_CANARY0 0x6865617062697264
#define HEAP_CANARY1 0x6269726468656170
#define HEAP_INITIAL_PAGES 1

typedef struct HeapSegmentStruct {
  u64 canary0;
  usz size;
  struct HeapSegmentStruct *prev;
  struct HeapSegmentStruct *next;
  u64 used;
  u64 canary1;
} __attribute__((packed)) HeapSegment;

void heap_init();
usz heap_get_used();
usz heap_get_free();
usz heap_get_overhead();

void heap_print_usage();

void *kmalloc(usz count);
void kfree(void *pointer);
