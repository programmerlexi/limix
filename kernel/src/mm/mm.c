#include "mm/mm.h"
#include "boot/limine.h"
#include "defines.h"
#include "mm/hhtp.h"
#include "types.h"
#include "utils/memory/memory.h"
#include "utils/memory/safety.h"

static memseg_t *_first = NULL;
static memseg_t *_last = NULL;

static void _insert_page(void *page) {
  nullsafe(page);
  memseg_t *seg = (memseg_t *)HHDM(page);
  if (_first == NULL) {
    seg->prev = NULL;
    seg->next = NULL;
    _first = seg;
    _last = seg;
  } else if ((uintptr_t)seg < (uintptr_t)_first) {
    seg->next = _first;
    _first->prev = seg;
    seg->prev = NULL;
    _first = seg;
  } else if ((uintptr_t)seg > (uintptr_t)_last) {
    seg->next = NULL;
    seg->prev = _last;
    _last->next = seg;
    _last = seg;
  } else {
    memseg_t *c = _first->next;
    memseg_t *p = _first;
    while (c != NULL) {
      if ((uintptr_t)p == (uintptr_t)seg)
        return;
      if ((uintptr_t)p < (uintptr_t)seg) {
        if ((uintptr_t)c > (uintptr_t)seg) {
          p->next = seg;
          c->prev = seg;
          seg->prev = p;
          seg->next = c;
          break;
        }
      }
      p = c;
      c = c->next;
    }
  }
}

BOOL mm_init(struct limine_memmap_response *mmap) {
  i32 usable = 0;
  for (u64 i = 0; i < mmap->entry_count; i++) {
    if (mmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      usable++;
      for (u64 j = 0; j < mmap->entries[i]->length; j += 0x1000) {
        _insert_page((void *)(mmap->entries[i]->base + j));
      }
    }
  }
  return usable != 0;
}

void *request_page() {
  if (_first == NULL)
    return NULL;
  memseg_t *s = _first;
  if (_last == _first) {
    _last = NULL;
  }
  _first = _first->next;
  kmemset(s, 0, 0x1000);
  return s;
}

void *request_page_block(usz n) {
  if (_first == NULL)
    return NULL;
  usz cl = 0;
  memseg_t *c = _first->next;
  memseg_t *p = _first;
  memseg_t *s = p;
  while (cl != n && c != NULL) {
    if (((uintptr_t)p + 0x1000) != (uintptr_t)c) {
      cl = 0;
      s = c;
    }
    p = c;
    c = c->next;
    cl++;
  }
  if (cl != n)
    return NULL;
  s->next = c;
  c->prev = s;
  if (s == _first)
    _first = c;
  if (s == _last)
    _last = s->prev;
  kmemset(s, 0, 0x1000 * n);
  return s;
}

void free_page(void *p) {
  nullsafe(p);
  kmemset(p, 0, 0x1000);
  _insert_page(p);
}

void free_page_block(void *p, usz n) {
  nullsafe(p);
  for (usz i = 0; i < n; i++) {
    free_page((void *)((uintptr_t)p + n * 0x1000));
  }
}
