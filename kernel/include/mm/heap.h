#pragma once

#include <stddef.h>
#include <stdint.h>
#include <types.h>

typedef struct heapseg {
  size_t size;
  struct heapseg *prev;
  struct heapseg *next;
  u64 used;
} heapseg_t;

void heap_init();
uint32_t heap_get_used();

void *malloc(size_t count);
void free(void *pointer);
