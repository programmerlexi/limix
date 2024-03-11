#include <boot/limine.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <stddef.h>
#include <stdint.h>
#include <utils/memory/memory.h>

memseg_t *first = NULL;
memseg_t *last = NULL;

void insert_page(void *page) {
  memseg_t *seg = (memseg_t *)HHDM(page);
  if (first == NULL) {
    seg->prev = NULL;
    seg->next = NULL;
    first = seg;
    last = seg;
  } else if ((uint64_t)seg < (uint64_t)first) {
    seg->next = first;
    first->prev = seg;
    seg->prev = NULL;
    first = seg;
  } else if ((uint64_t)seg > (uint64_t)last) {
    seg->next = NULL;
    seg->prev = last;
    last->next = seg;
    last = seg;
  } else {
    memseg_t *c = first->next;
    memseg_t *p = first;
    while (c != NULL) {
      if ((uint64_t)p == (uint64_t)seg)
        return;
      if ((uint64_t)p < (uint64_t)seg) {
        if ((uint64_t)c > (uint64_t)seg) {
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
  for (uint64_t i = 0; i < mmap->entry_count; i++) {
    if (mmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      for (uint64_t j = 0; j < mmap->entries[i]->length; j += 0x1000) {
        insert_page((void *)(mmap->entries[i]->base + j));
      }
    }
  }
  return true;
}

void *request_page() {
  if (first == NULL)
    return NULL;
  memseg_t *s = first;
  if (last == first) {
    last = NULL;
  } else {
    first = first->next;
  }
  return memset(s, 0, 0x1000);
}

void *request_pages(uint64_t n) {
  if (first == NULL)
    return NULL;
  uint64_t cl = 0;
  memseg_t *c = first->next;
  memseg_t *p = first;
  memseg_t *s = p;
  while (cl != n && c != NULL) {
    if (((uint64_t)p + 0x1000) != (uint64_t)c) {
      cl = 0;
      s = c;
    }
    p = c;
    c = c->next;
    cl++;
  }
  if (cl != (n + 1))
    return NULL;
  s->prev->next = c;
  c->prev = s->prev;
  return memset(s, 0, 0x1000 * n);
}

void free_page(void *p) {
  memset(p, 0, 0x1000);
  insert_page(p);
}

void free_page_block(void *p, uint64_t n) {
  for (uint64_t i = 0; i < n; i++) {
    free_page((void *)((uint64_t)p + n * 0x1000));
  }
}
