#include "mm/heap.h"
#include "config.h"
#include "defines.h"
#include "kernel.h"
#include "mm/mm.h"
#include "types.h"
#include "utils/memory/memory.h"
#include "utils/memory/safety.h"

static heapseg_t *_heap_first;
static heapseg_t *_heap_last;

void heap_init() {
  _heap_first = request_page_block(CONFIG_HEAP_INITIAL_PAGES);
  if (!_heap_first)
    kernel_panic_error("Not enough memory for heap");
  _heap_last = _heap_first;
  kmemset(_heap_first, 0, CONFIG_HEAP_INITIAL_PAGES * 0x1000);
  _heap_first->next = NULL;
  _heap_first->prev = NULL;
  _heap_first->size = (CONFIG_HEAP_INITIAL_PAGES * 0x1000) - sizeof(heapseg_t);
  _heap_first->used = FALSE;
}

static void _heap_combine_forward(heapseg_t *seg) {
  nullsafe(seg->next);
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
  if (seg->prev != NULL)
    if (!seg->prev->used)
      _heap_combine_forward(seg->prev);
}

void expand_heap(usz size) {
  if (size % 0x1000 > 0) {
    size -= size % 0x1000;
    size += 0x1000;
  }
  usz pages = size / 0x1000;
  heapseg_t *new_seg = (heapseg_t *)request_page_block(pages);
  nullsafe_error(new_seg, "No more heap space");
  new_seg->used = FALSE;
  new_seg->prev = _heap_last;
  _heap_last->next = new_seg;
  _heap_last = new_seg;
  new_seg->next = NULL;
  new_seg->size = size - sizeof(heapseg_t);
  _heap_combine_backward(new_seg);
}

static heapseg_t *_heap_split(heapseg_t *seg, usz size) {
  if (size < 0x10)
    return NULL;
  usz splitSize = seg->size - size - sizeof(heapseg_t);
  if (splitSize < 0x10)
    return NULL;
  heapseg_t *new_seg = (heapseg_t *)((usz)seg + size + sizeof(heapseg_t));

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
  heapseg_t *seg = (heapseg_t *)addr - 1;
  seg->used = FALSE;
  _heap_combine_forward(seg);
  _heap_combine_backward(seg);
}

void *kmalloc(usz size) {
  if (size % 0x10 > 0) {
    size -= size % 0x10;
    size += 0x10;
  }
  if (size == 0)
    return NULL;
  heapseg_t *seg = _heap_first;
  while (TRUE) {
    if (!seg->used) {
      if (seg->size > (size + sizeof(heapseg_t))) {
        _heap_split(seg, size);
        seg->used = TRUE;
        return (void *)((usz)seg + sizeof(heapseg_t));
      }
      if (seg->size >= size) {
        seg->used = TRUE;
        return (void *)((usz)seg + sizeof(heapseg_t));
      }
    }
    if (seg->next == NULL)
      break;
    seg = seg->next;
  }
  expand_heap((size + sizeof(heapseg_t)) * 2);
  return kmalloc(size);
}

void *calloc(usz size, u8 val) {
  void *ptr = kmalloc(size);
  if (size % 0x10 > 0) {
    size -= size % 0x10;
    size += 0x10;
  }
  kmemset(ptr, val, size);
  return ptr;
}

u32 heap_get_used() {
  u32 used = 0;
  heapseg_t *c = _heap_first;
  while (c) {
    used += sizeof(heapseg_t);
    if (c->used)
      used += c->size;
    c = c->next;
  }
  return used;
}
