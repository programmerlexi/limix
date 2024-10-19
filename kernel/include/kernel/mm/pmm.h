#pragma once

#include <libk/types.h>
#include <stddef.h>

#define PMM_MAX_TAGS_PER_ZONE 256

#define PMM_ZONE_RESERVE 0
#define PMM_ZONE_DMA 1
#define PMM_ZONE_KERNEL 2
#define PMM_ZONE_NORMAL 3
#define PMM_ZONE_HIGHMEM 4

typedef struct {
  u64 start_index;
  u64 length;
} MemoryTag;

typedef struct {
  u64 start_index;
  u64 end_index;
  u64 used_tags;
  MemoryTag tags[PMM_MAX_TAGS_PER_ZONE];
} MemoryZone;

void pmm_init();

void *request_dma(u64 pages);
void *request_kernel(u64 pages);
void *request_normal(u64 pages);

// Frees a block of pages
void free_pages(void *physical, size_t count);

// Marks a block of pages as used
void mark_pages(void *start, size_t count);
