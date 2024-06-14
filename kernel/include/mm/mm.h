#pragma once
#include "libk/types.h"
#include "limine.h"
#include <stdbool.h>

typedef struct memseg {
  struct memseg *prev;
  struct memseg *next;
  usz pages;
} memseg_t;

bool mm_init(struct limine_memmap_response *);

uptr mm_fix(uptr ptr);

void *request_page();
void *request_page_block(usz count);

void free_page(void *addr);
void free_page_block(void *addr, usz count);
