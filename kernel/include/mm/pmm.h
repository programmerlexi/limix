#pragma once

#include "libk/types.h"
#include <stddef.h>

typedef struct {
  i64 start_index;
  i64 length;
} MemoryTag;

void pmm_init();

// Returns the physical address of a page
void *request_page();

// Returns the physical address of a page block of specified size
void *request_pages(size_t count);

// Frees a page
void free_page(void *physical);

// Frees a block of pages
void free_pages(void *physical, size_t count);

// Marks a block of pages as used
void mark_pages(void *start, size_t count);
