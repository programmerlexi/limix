#pragma once
#include <boot/limine.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct memseg {
  struct memseg *prev;
  struct memseg *next;
} memseg_t;

bool mm_init(struct limine_memmap_response *);

void *request_page();
void *request_page_block(size_t);

void free_page(void *);
void free_page_block(void *, size_t);
