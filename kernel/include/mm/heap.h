#pragma once

#include "types.h"
#include <stddef.h>
#include <stdint.h>

typedef struct heapseg {
  size_t size;
  struct heapseg *prev;
  struct heapseg *next;
  u64 used;
} heapseg_t;

void heap_init();
uint32_t heap_get_used();

void *kmalloc(size_t count);
void kfree(void *pointer);
