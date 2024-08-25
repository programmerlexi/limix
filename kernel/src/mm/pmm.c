#include "kernel/mm/pmm.h"
#include "kernel/boot/requests.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "libk/math/lib.h"
#include "libk/utils/memory/safety.h"
#include "limine.h"

u8 *page_bitmap;
u64 page_bitmap_size;
u64 memory_size;

static void _pmm_bitmap_set(u64 i) {
  if ((i / 8) > page_bitmap_size)
    return;
  u64 byi = i / 8;
  u64 bti = i % 8;
  page_bitmap[byi] |= 1 << bti;
}

static void _pmm_bitmap_reset(u64 i) {
  if ((i / 8) > page_bitmap_size)
    return;
  u64 byi = i / 8;
  u64 bti = i % 8;
  page_bitmap[byi] &= ~(1 << bti);
}

static bool _pmm_bitmap_get(u64 i) {
  if ((i / 8) > page_bitmap_size)
    return true;
  u64 byi = i / 8;
  u64 bti = i % 8;
  return page_bitmap[byi] & (1 << bti);
}

void mark_pages(void *start, size_t count) {
  for (u64 i = 0; i < count; i++) {
    _pmm_bitmap_set(((u64)start >> 12) + i);
  }
}

void pmm_init() {
  if (!g_mmap_request.response)
    kernel_panic_error("No memory map response");

  // Count memory
  memory_size = 0;
  for (u64 i = 0; i < g_mmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *e = g_mmap_request.response->entries[i];
    memory_size = max(memory_size, e->base + e->length);
  }
  page_bitmap_size = memory_size >> 15;

  // Allocate memory for the memory allocator
  struct limine_memmap_entry *fit = NULL;
  for (u64 i = 0; i < g_mmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *e = g_mmap_request.response->entries[i];
    if (e->type != LIMINE_MEMMAP_USABLE)
      continue;
    if (e->length >= page_bitmap_size) {
      fit = e;
      break;
    }
  }

  if (!fit)
    kernel_panic_error("Not enough memory for memory bitmap");

  fit->length -= page_bitmap_size;
  fit->length = (fit->length >> 12) << 12;
  page_bitmap = (void *)HHDM(fit->base + fit->length);

  // Mark memory
  u64 end_last = 0;
  for (u64 i = 0; i < g_mmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *e = g_mmap_request.response->entries[i];
    if (e->base != end_last) {
      mark_pages((void *)end_last, (e->base - end_last) >> 12);
    }
    end_last = e->base + e->length;
    if (e->type == LIMINE_MEMMAP_USABLE)
      free_pages((void *)((e->base >> 12) << 12), e->length >> 12);
    else
      mark_pages((void *)((e->base >> 12) << 12), e->length >> 12);
  }
}

u64 last = 0;

void free_page(void *addr) {
  if (mm_is_virtual((uptr)addr))
    kernel_panic_error("Got virtual address instead of physical (please fix)");
  _pmm_bitmap_reset((u64)addr >> 12);
  last = min(((u64)addr >> 12), last);
}

void free_pages(void *physical, size_t count) {
  for (u64 i = 0; i < count; i++) {
    free_page((void *)((u64)physical + (i << 12)));
  }
}

void *request_page() {
  for (u64 i = last; i < page_bitmap_size * 8; i++) {
    if (!_pmm_bitmap_get(i)) {
      _pmm_bitmap_set(i);
      last = i;
      return (void *)(i << 12);
    }
  }
  if (last != 0) {
    last = 0;
    return request_page();
  }
  kernel_panic_error("Out of memory (physical)");
  return NULL;
}

void *request_pages(size_t count) {
  u64 c = 0;
  for (u64 i = last; i < page_bitmap_size * 8; i++) {
    if (!_pmm_bitmap_get(i))
      c++;
    else
      c = 0;
    if (c == count) {
      last = i;
      mark_pages((void *)((i - c) << 12), c);
      return (void *)((i - c) << 12);
    }
  }
  if (last != 0) {
    last = 0;
    return request_pages(count);
  }
  kernel_panic_error("Out of memory (physical)");
  return NULL;
}
