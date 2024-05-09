#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct heapseg {
  size_t size;
  struct heapseg *prev;
  struct heapseg *next;
  uint64_t used;
} heapseg_t;

void heap_init();
uint32_t heap_get_used();

void *malloc(size_t count);
void free(void *pointer);
