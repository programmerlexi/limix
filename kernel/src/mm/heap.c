#include <config.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <mm/heap.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/memory/memory.h>
#include <utils/memory/safety.h>

static heapseg_t *heap_first;
static heapseg_t *heap_last;

void heap_init() {
  heap_first = request_page_block(CONFIG_HEAP_INITIAL_PAGES);
  if (!heap_first)
    kernel_panic_error("Not enough memory for heap");
  heap_last = heap_first;
  memset(heap_first, 0, CONFIG_HEAP_INITIAL_PAGES * 0x1000);
  heap_first->next = NULL;
  heap_first->prev = NULL;
  heap_first->size = (CONFIG_HEAP_INITIAL_PAGES * 0x1000) - sizeof(heapseg_t);
  heap_first->used = false;
}

static void _heap_combine_forward(heapseg_t *seg) {
  nullsafe(seg->next);
  if (seg->next->used)
    return;
  if ((uintptr_t)(seg->next) !=
      ((uintptr_t)seg + sizeof(heapseg_t) + seg->size))
    return;
  if (seg->next == heap_last)
    heap_last = seg;
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

void expand_heap(size_t size) {
  if (size % 0x1000 > 0) {
    size -= size % 0x1000;
    size += 0x1000;
  }
  size_t pages = size / 0x1000;
  heapseg_t *new_seg = (heapseg_t *)request_page_block(pages);
  nullsafe_with(new_seg, { kernel_panic_error("No more heap space"); });
  new_seg->used = false;
  new_seg->prev = heap_last;
  heap_last->next = new_seg;
  heap_last = new_seg;
  new_seg->next = NULL;
  new_seg->size = size - sizeof(heapseg_t);
  _heap_combine_backward(new_seg);
}

static heapseg_t *_heap_split(heapseg_t *seg, size_t size) {
  if (size < 0x10)
    return NULL;
  size_t splitSize = seg->size - size - sizeof(heapseg_t);
  if (splitSize < 0x10)
    return NULL;
  heapseg_t *new_seg = (heapseg_t *)((size_t)seg + size + sizeof(heapseg_t));

  if (seg->next)
    seg->next->prev = new_seg;
  new_seg->next = seg->next;
  seg->next = new_seg;
  new_seg->prev = seg;
  new_seg->used = seg->used;
  new_seg->size = splitSize;
  seg->size = size;
  if (heap_last == seg)
    heap_last = new_seg;
  return new_seg;
}

void free(void *addr) {
  nullsafe(addr);
  heapseg_t *seg = (heapseg_t *)addr - 1;
  seg->used = false;
  _heap_combine_forward(seg);
  _heap_combine_backward(seg);
}

void *malloc(size_t size) {
  if (size % 0x10 > 0) {
    size -= size % 0x10;
    size += 0x10;
  }
  if (size == 0)
    return NULL;
  heapseg_t *seg = heap_first;
  while (true) {
    if (!seg->used) {
      if (seg->size > (size + sizeof(heapseg_t))) {
        _heap_split(seg, size);
        seg->used = true;
        return (void *)((size_t)seg + sizeof(heapseg_t));
      }
      if (seg->size >= size) {
        seg->used = true;
        return (void *)((size_t)seg + sizeof(heapseg_t));
      }
    }
    if (seg->next == NULL)
      break;
    seg = seg->next;
  }
  expand_heap((size + sizeof(heapseg_t)) * 2);
  return malloc(size);
}

void *calloc(size_t size, uint8_t val) {
  void *ptr = malloc(size);
  if (size % 0x10 > 0) {
    size -= size % 0x10;
    size += 0x10;
  }
  memset(ptr, val, size);
  return ptr;
}
