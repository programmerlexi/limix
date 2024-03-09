#include "limine.h"
#include <mm.h>
#include <stddef.h>
#include <stdint.h>

memseg_t *first;
memseg_t *last;

bool mm_init(struct limine_memmap_response *mmap) {
  for (uint64_t i = 0; i < mmap->entry_count; i++) {
    if (mmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      for (uint64_t j = 0; j < mmap->entries[i]->length; j += 0x1000) {
        memseg_t *seg = (memseg_t *)(mmap->entries[i]->base + j);
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
    }
  }
  return true;
}
