#include <boot/limine.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <stddef.h>
#include <stdint.h>
#include <utils/memory/memory.h>
#include <utils/memory/safety.h>

static memseg_t *first = NULL;
static memseg_t *last = NULL;

static void insert_page(void *page) {
  nullsafe(page);
  memseg_t *seg = (memseg_t *)HHDM(page);
  if (first == NULL) {
    seg->prev = NULL;
    seg->next = NULL;
    first = seg;
    last = seg;
  } else if ((uintptr_t)seg < (uintptr_t)first) {
    seg->next = first;
    first->prev = seg;
    seg->prev = NULL;
    first = seg;
  } else if ((uintptr_t)seg > (uintptr_t)last) {
    seg->next = NULL;
    seg->prev = last;
    last->next = seg;
    last = seg;
  } else {
    memseg_t *c = first->next;
    memseg_t *p = first;
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

bool mm_init(struct limine_memmap_response *mmap) {
  int usable = 0;
  for (uint64_t i = 0; i < mmap->entry_count; i++) {
    if (mmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      usable++;
      for (uint64_t j = 0; j < mmap->entries[i]->length; j += 0x1000) {
        insert_page((void *)(mmap->entries[i]->base + j));
      }
    }
  }
  return usable != 0;
}

void *request_page() {
  if (first == NULL)
    return NULL;
  memseg_t *s = first;
  if (last == first) {
    last = NULL;
  }
  first = first->next;
  memset(s, 0, 0x1000);
  return s;
}

void *request_page_block(size_t n) {
  if (first == NULL)
    return NULL;
  size_t cl = 0;
  memseg_t *c = first->next;
  memseg_t *p = first;
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
  if (s == first)
    first = c;
  if (s == last)
    last = s->prev;
  memset(s, 0, 0x1000 * n);
  return s;
}

void free_page(void *p) {
  nullsafe(p);
  memset(p, 0, 0x1000);
  insert_page(p);
}

void free_page_block(void *p, size_t n) {
  nullsafe(p);
  for (size_t i = 0; i < n; i++) {
    free_page((void *)((uintptr_t)p + n * 0x1000));
  }
}
