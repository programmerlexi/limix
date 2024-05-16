#pragma once
#include "boot/limine.h"
#include "types.h"
#include <stdbool.h>

typedef struct memseg {
  struct memseg *prev;
  struct memseg *next;
} memseg_t;

bool mm_init(struct limine_memmap_response *);

void *request_page();
void *request_page_block(usz count);

void free_page(void *addr);
void free_page_block(void *addr, usz count);
