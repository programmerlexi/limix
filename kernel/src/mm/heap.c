#include "kernel/mm/heap.h"
#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "kernel/mm/vmm.h"
#include "libk/ipc/spinlock.h"
#include "libk/types.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/memory/safety.h"
#include <stdbool.h>
#include <stddef.h>

static heapseg_t *_heap_first;
static heapseg_t *_heap_last;

static u32 _heap_lock;

static uptr heap_top;

static void _heap_check_canary(heapseg_t *seg) {
  nullsafe(seg);
  if (seg->canary0 != HEAP_CANARY0)
    kernel_panic_error("Heap canary0 invalid");
  if (seg->canary1 != HEAP_CANARY1)
    kernel_panic_error("Heap canary1 invalid");
}

void heap_init() {
  uptr heap_space = PHY(request_page_block(CONFIG_HEAP_INITIAL_PAGES));
  for (int i = 0; i < CONFIG_HEAP_INITIAL_PAGES; i++)
    vmm_map((void *)(HEAP_BASE + (i << 12)), (void *)(heap_space + (i << 12)),
            VMM_WRITEABLE);

  _heap_first = (void *)HEAP_BASE;
  _heap_last = _heap_first;
  kmemset(_heap_first, 0, CONFIG_HEAP_INITIAL_PAGES * 0x1000);
  _heap_first->next = NULL;
  _heap_first->prev = NULL;
  _heap_first->size = (CONFIG_HEAP_INITIAL_PAGES * 0x1000) - sizeof(heapseg_t);
  _heap_first->used = false;
  _heap_first->canary0 = HEAP_CANARY0;
  _heap_first->canary1 = HEAP_CANARY1;
  heap_top = HEAP_BASE + (CONFIG_HEAP_INITIAL_PAGES << 12);
}

static void _heap_combine_forward(heapseg_t *seg) {
  nullsafe(seg);
  _heap_check_canary(seg);
  nullsafe(seg->next);
  _heap_check_canary(seg->next);
  if (seg->next->used)
    return;
  if ((uintptr_t)(seg->next) !=
      ((uintptr_t)seg + sizeof(heapseg_t) + seg->size))
    return;
  if (seg->next == _heap_last)
    _heap_last = seg;
  if (seg->next->next != NULL)
    seg->next->next->prev = seg;
  seg->size = seg->size + seg->next->size + sizeof(heapseg_t);
  seg->next = seg->next->next;
}

static void _heap_combine_backward(heapseg_t *seg) {
  nullsafe(seg);
  _heap_check_canary(seg);
  nullsafe(seg->prev);
  _heap_check_canary(seg->prev);
  if (seg->prev->used)
    return;
  _heap_combine_forward(seg->prev);
}

void expand_heap(usz size) {
  if (size == 0)
    return;
  if (size % 0x1000 > 0) {
    size -= size % 0x1000;
    size += 0x1000;
  }
  usz pages = size / 0x1000;
  for (usz i = 0; i < pages; i++) {
    uptr new_space = PHY(request_page());
    vmm_map((void *)heap_top, (void *)new_space, VMM_WRITEABLE);
    heap_top += 0x1000;
  }
  heapseg_t *new_seg =
      (heapseg_t *)((uptr)_heap_last + sizeof(*_heap_last) + _heap_last->size);
  new_seg->used = false;
  new_seg->prev = _heap_last;
  new_seg->canary0 = HEAP_CANARY0;
  new_seg->canary1 = HEAP_CANARY1;
  _heap_last->next = new_seg;
  _heap_last = new_seg;
  new_seg->next = NULL;
  new_seg->size = size - sizeof(heapseg_t);
  _heap_combine_backward(new_seg);
}

static heapseg_t *_heap_split(heapseg_t *seg, usz size) {
  nullsafe_error(seg, "Heap split with no segment");
  _heap_check_canary(seg);
  if (size < 0x10)
    return NULL;
  usz splitSize = seg->size - size - sizeof(heapseg_t);
  if (splitSize < 0x10)
    return NULL;
  heapseg_t *new_seg = (heapseg_t *)((usz)seg + size + sizeof(heapseg_t));
  new_seg->canary0 = HEAP_CANARY0;
  new_seg->canary1 = HEAP_CANARY1;

  if (seg->next)
    seg->next->prev = new_seg;
  new_seg->next = seg->next;
  seg->next = new_seg;
  new_seg->prev = seg;
  new_seg->used = seg->used;
  new_seg->size = splitSize;
  seg->size = size;
  if (_heap_last == seg)
    _heap_last = new_seg;
  return new_seg;
}

void kfree(void *addr) {
  nullsafe(addr);
  spinlock(&_heap_lock);
  heapseg_t *seg = (heapseg_t *)addr - 1;
  _heap_check_canary(seg);
  if (!seg->used)
    log(LOGLEVEL_WARN2, "Encountered double free");
  seg->used = false;
  _heap_combine_forward(seg);
  _heap_combine_backward(seg);
  spinunlock(&_heap_lock);
}

void *kmalloc(usz size) {
  if (size % 0x10 > 0) {
    size -= size % 0x10;
    size += 0x10;
  }
  if (size == 0)
    return NULL;
  spinlock(&_heap_lock);
  heapseg_t *seg = _heap_first;
  while (true) {
    if (!seg)
      kernel_panic_error("Heap segment missing");
    _heap_check_canary(seg);
    if (!seg->used) {
      if (seg->size > (size + sizeof(heapseg_t) * 2)) {
        _heap_split(seg, size);
        seg->used = true;
        spinunlock(&_heap_lock);
        return (void *)((usz)seg + sizeof(heapseg_t));
      }
      if (seg->size >= size) {
        seg->used = true;
        spinunlock(&_heap_lock);
        return (void *)((usz)seg + sizeof(heapseg_t));
      }
    }
    if (seg->next == NULL)
      break;
    seg = seg->next;
  }
  expand_heap((size + sizeof(heapseg_t)));
  spinunlock(&_heap_lock);
  return kmalloc(size);
}

void *kcalloc(usz size) {
  void *ptr = kmalloc(size);
  kmemset(ptr, 0, size);
  return ptr;
}

u32 heap_get_used() {
  u32 used = 0;
  heapseg_t *c = _heap_first;
  while (c) {
    _heap_check_canary(c);
    used += sizeof(heapseg_t);
    if (c->used)
      used += c->size;
    c = c->next;
  }
  return used;
}
