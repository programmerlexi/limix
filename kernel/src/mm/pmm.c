#include <kernel/boot/requests.h>
#include <kernel/debug.h>
#include <kernel/kernel.h>
#include <kernel/mm/hhtp.h>
#include <kernel/mm/mm.h>
#include <kernel/mm/pmm.h>
#include <libk/math/lib.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/memory/safety.h>
#include <limine.h>

#define MAX_TAGS 256

MemoryTag *memory_tags;
u64 memory_tag_list_size;
u64 memory_tag_count;
u64 memory_size;

void _fix_memtags() {
  u64 changes = 0;
  for (u64 i = 0; i < memory_tag_count; i++) {
    MemoryTag *memtag = &memory_tags[i];
    if (memtag->length)
      continue;
    kmemmove(memtag, memtag + 1, sizeof(MemoryTag) * (memory_tag_count - i));
    i--;
    memory_tag_count--;
    changes++;
  }
  for (u64 i = 0; i < memory_tag_count - 1; i++) {
    for (u64 j = i + 1; j < memory_tag_count; j++) {
      MemoryTag *memtag0 = &memory_tags[i];
      MemoryTag *memtag1 = &memory_tags[j];
      i64 sa = memtag0->start_index;
      i64 sb = memtag1->start_index;
      i64 ea = memtag0->start_index + memtag0->length;
      i64 eb = memtag1->start_index + memtag1->length;
      if (sb < sa)
        continue;
      if (sb <= ea) {
        if (ea < eb)
          ea = eb;
        kmemmove(memtag1, memtag1 + 1,
                 sizeof(MemoryTag) * (memory_tag_count - (i + 1)));
        j--;
        memory_tag_count--;
        memtag0->start_index = sa;
        memtag0->length = ea - sa;
        changes++;
        continue;
      }
    }
  }
  if (changes)
    _fix_memtags();
}

void mark_pages(void *start, size_t c) {
  i64 idx = (u64)start >> 12;
  i64 count = c;
  for (u64 i = 0; i < memory_tag_count; i++) {
    MemoryTag *memtag = &memory_tags[i];
    if (memtag->start_index > idx)
      continue;
    if (memtag->start_index + memtag->length < idx)
      continue;
    if (idx == memtag->start_index) {
      memtag->start_index += count;
      memtag->length = max(memtag->length - count, 0);
      _fix_memtags();
      return;
    }
    if (idx == memtag->start_index + memtag->length) {
      memtag->length = max(memtag->length - count, 0);
      _fix_memtags();
      return;
    }
    i64 ri = idx - memtag->start_index;
    if (ri > memtag->length)
      continue;
    if (ri + count > memtag->length) {
      memtag->length = ri;
      _fix_memtags();
      return;
    }
    kernel_panic_error("Splitting tags is not yet implemented");
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
  memory_tag_list_size = MAX_TAGS * sizeof(MemoryTag);

  // Allocate memory for the memory allocator
  struct limine_memmap_entry *fit = NULL;
  for (u64 i = 0; i < g_mmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *e = g_mmap_request.response->entries[i];
    if (e->type != LIMINE_MEMMAP_USABLE)
      continue;
    if (e->length >= memory_tag_list_size) {
      fit = e;
      break;
    }
  }

  if (!fit)
    kernel_panic_error("Not enough memory for memory tags");

  fit->length -= memory_tag_list_size;
  fit->length = (fit->length >> 12) << 12;
  memory_tags = (void *)HHDM(fit->base + fit->length);

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

void free_page(void *addr) {
  if (mm_is_virtual((uptr)addr))
    kernel_panic_error("Got virtual address instead of physical (please fix)");
  free_pages(addr, 1);
}

void free_pages(void *physical, size_t count) {
  if (mm_is_virtual((uptr)physical))
    kernel_panic_error("Got virtual address instead of physical (please fix)");
  i64 idx = (i64)physical >> 12;
  if (memory_tag_count >= MAX_TAGS)
    kernel_panic_error("Out of tags");
  memory_tag_count++;
  memory_tags[memory_tag_count - 1].start_index = idx;
  memory_tags[memory_tag_count - 1].length = count;
  _fix_memtags();
}

void *request_page() {
  if (memory_tag_count) {
    void *a = (void *)(memory_tags[0].start_index << 12);
    mark_pages(a, 1);
    return a;
  }
  kernel_panic_error("Out of memory (physical)");
  return NULL;
}

void *request_pages(size_t c) {
  i64 count = c;
  for (u64 i = 0; i < memory_tag_count; i++) {
    MemoryTag *memory_tag = &memory_tags[i];
    if (memory_tag->length < count)
      continue;
    void *a = (void *)(memory_tag->start_index << 12);
    mark_pages(a, count);
    return a;
  }
  kernel_panic_error("Out of memory (physical)");
  return NULL;
}
