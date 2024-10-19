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

static MemoryZone zones[5];
static size_t memory_size;

void _fix_memtags(MemoryZone *zone) {
  u64 changes = 0;
  if (!zone->used_tags)
    return;
  for (u64 i = 0; i < zone->used_tags; i++) {
    u64 s = zone->tags[i].start_index;
    u64 e = zone->tags[i].start_index + zone->tags[i].length;
    s = min(max(s, zone->start_index), zone->end_index);
    e = min(max(e, zone->start_index), zone->end_index);
    zone->tags[i].start_index = s;
    zone->tags[i].length = e - s;
  }
  if (!zone->used_tags)
    return;
  for (u64 i = 0; i < zone->used_tags; i++) {
    MemoryTag *memtag = &zone->tags[i];
    if (memtag->length)
      continue;
    kmemmove(memtag, memtag + 1, sizeof(MemoryTag) * (zone->used_tags - i));
    i--;
    zone->used_tags--;
    changes++;
  }
  if (!zone->used_tags)
    return;
  for (u64 i = 0; i < zone->used_tags - 1; i++) {
    for (u64 j = i + 1; j < zone->used_tags; j++) {
      MemoryTag *memtag0 = &zone->tags[i];
      MemoryTag *memtag1 = &zone->tags[j];
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
                 sizeof(MemoryTag) * (zone->used_tags - (i + 1)));
        j--;
        zone->used_tags--;
        memtag0->start_index = sa;
        memtag0->length = ea - sa;
        changes++;
        continue;
      }
    }
  }
  if (changes)
    _fix_memtags(zone);
}

static void mark_pages_in_zone(MemoryZone *zone, u64 idx, u64 c) {
  for (u64 i = 0; i < zone->used_tags; i++) {
    MemoryTag *memtag = &zone->tags[i];
    if (memtag->start_index > idx)
      continue;
    if (memtag->start_index + memtag->length < idx)
      continue;
    if (idx == memtag->start_index) {
      memtag->start_index += c;
      memtag->length = max((i64)(memtag->length - c), 0);
      _fix_memtags(zone);
      return;
    }
    if (idx == memtag->start_index + memtag->length - c) {
      memtag->length = max((i64)(memtag->length - c), 0);
      _fix_memtags(zone);
      return;
    }
    u64 ri = idx - memtag->start_index;
    if (ri > memtag->length)
      continue;
    if (ri + c > memtag->length) {
      memtag->length = ri;
      _fix_memtags(zone);
      return;
    }
    kernel_panic_error("Splitting tags is not implemented yet.");
  }
}

void mark_pages(void *start, size_t c) {
  for (u64 i = 0; i < 5; i++) {
    mark_pages_in_zone(&zones[i], (u64)start >> 12, c);
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

  // Initialize zones
  zones[PMM_ZONE_RESERVE].start_index = 0;
  zones[PMM_ZONE_RESERVE].end_index = 0x100;
  zones[PMM_ZONE_DMA].start_index = 0x100;
  zones[PMM_ZONE_DMA].end_index = 0x40000;
  zones[PMM_ZONE_KERNEL].start_index = 0x40000;
  zones[PMM_ZONE_KERNEL].end_index = 0x80000;
  zones[PMM_ZONE_NORMAL].start_index = 0x80000;
  zones[PMM_ZONE_NORMAL].end_index = 0x100000;
  zones[PMM_ZONE_HIGHMEM].start_index = 0x100000;
  zones[PMM_ZONE_HIGHMEM].end_index = max(memory_size >> 12, 0x100000);

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

void free_pages(void *physical, size_t count) {
  if (mm_is_virtual((uptr)physical))
    kernel_panic_error("Got virtual address instead of physical (please fix)");
  u64 idx = (u64)physical >> 12;
  for (u64 i = 0; i < 5; i++) {
    if (zones[i].used_tags >= PMM_MAX_TAGS_PER_ZONE)
      kernel_panic_error("Out of free tags");
    zones[i].used_tags++;
    zones[i].tags[zones[i].used_tags - 1].start_index = idx;
    zones[i].tags[zones[i].used_tags - 1].length = count;
    _fix_memtags(&zones[i]);
  }
}

static void *request_pages_in_zone(MemoryZone *zone, u64 count) {
  for (u64 i = 0; i < zone->used_tags; i++) {
    MemoryTag *tag = &zone->tags[i];
    if (tag->length < count)
      continue;
    void *a = (void *)(tag->start_index << 12);
    tag->length -= count;
    tag->start_index += count;
    _fix_memtags(zone);
    kmemset((void *)HHDM(a), 0, 0x1000);
    return a;
  }
  return NULL;
}

void *request_dma(u64 pages) {
  void *addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_DMA], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_NORMAL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_KERNEL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_RESERVE], pages);
  if (addr)
    return addr;
  kernel_panic_error("Out of DMA memory");
}

void *request_kernel(u64 pages) {
  void *addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_KERNEL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_NORMAL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_HIGHMEM], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_RESERVE], pages);
  if (addr)
    return addr;
  kernel_panic_error("Out of kernel memory");
}

void *request_normal(u64 pages) {
  void *addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_HIGHMEM], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_NORMAL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_KERNEL], pages);
  if (addr)
    return addr;
  addr = request_pages_in_zone(&zones[PMM_ZONE_RESERVE], pages);
  if (addr)
    return addr;
  kernel_panic_error("Out of normal memory");
}
