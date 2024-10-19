#include <kernel/kernel.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/hhtp.h>
#include <kernel/mm/pmm.h>
#include <libk/utils/memory/memory.h>

static HeapSegment *heap_start;
static HeapSegment *heap_end;

static u64 used;
static u64 free;
static u64 overhead;

static void _heap_validate(HeapSegment *s) {
  if (s->canary0 != HEAP_CANARY0)
    kernel_panic_error("Heap canary0");
  if (s->canary1 != HEAP_CANARY1)
    kernel_panic_error("Heap canary0");
}

static void _heap_combine(HeapSegment *s) {
  if (!s)
    return;
  _heap_validate(s);
  if (s->used)
    return;
  HeapSegment *n = s->next;
  if (!n)
    return;
  if (n->used)
    return;
  if ((u64)s + sizeof(*s) + s->size != (u64)n)
    return;
  s->size += sizeof(*n) + n->size;
  s->next = n->next;
  if (s->next)
    s->next->prev = s;

  used -= sizeof(*s);
  overhead -= sizeof(*s);
  free += sizeof(*s);

  _heap_combine(s->next);
  _heap_combine(s->prev);
}
static void _heap_cut(HeapSegment *s, usz c) {
  _heap_validate(s);
  if (c % 16)
    c += 16 - (c % 16);
  if ((i64)(s->size - c - 16 - sizeof(*s)) < 0)
    return;
  HeapSegment *n = (HeapSegment *)((u64)s + sizeof(*s) + c);
  n->size = s->size - sizeof(*n) - c;
  s->size = c;
  if (s->next)
    s->next->prev = n;
  n->next = s->next;
  n->prev = s;
  s->next = n;

  n->canary0 = HEAP_CANARY0;
  n->canary1 = HEAP_CANARY1;

  used += sizeof(*s);
  overhead += sizeof(*s);
  free -= sizeof(*s);
}

void heap_init() {
  heap_start = (void *)HHDM(request_kernel(HEAP_INITIAL_PAGES));
  heap_end = heap_start;
  heap_start->size = HEAP_INITIAL_PAGES * 0x1000 - sizeof(*heap_start);
  heap_start->canary0 = HEAP_CANARY0;
  heap_start->canary1 = HEAP_CANARY1;
}

usz heap_get_used() { return used; }
usz heap_get_free() { return free; }
usz heap_get_overhead() { return overhead; }

void *kmalloc(usz count) {
  for (HeapSegment *s = heap_start; s != NULL; s = s->next) {
    _heap_validate(s);
    if (s->size < count)
      continue;
    if (s->used)
      continue;
    if (s->size > count)
      _heap_cut(s, count);
    s->used = true;
    kmemset((void *)((u64)s + sizeof(*s)), 0, count);
    return (void *)((u64)s + sizeof(*s));
  }
  HeapSegment *s = (void *)HHDM(request_kernel(count / 0x1000 + 1));
  s->prev = heap_end;
  heap_end->next = s;
  s->size = (count / 0x1000 + 1) - sizeof(*s);
  s->canary0 = HEAP_CANARY0;
  s->canary1 = HEAP_CANARY1;
  return kmalloc(count);
}
void kfree(void *pointer) {
  for (HeapSegment *s = heap_start; s != NULL; s = s->next) {
    _heap_validate(s);
    if ((u64)s + sizeof(*s) + s->size < (u64)pointer)
      continue;
    if ((u64)s > (u64)pointer)
      continue;
    s->used = false;
    _heap_combine(s);
    return;
  }
}
