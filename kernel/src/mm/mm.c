#include "kernel/mm/mm.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"
#include "libk/ipc/spinlock.h"
#include "libk/types.h"
#include "libk/utils/memory/safety.h"
#include "limine.h"
#include <stdbool.h>
#include <stddef.h>

static memseg_t *_first;
static memseg_t *_last;
static u32 page_lock;

#define lock spinlock(&page_lock);
#define unlock spinunlock(&page_lock);

uptr mm_fix(uptr ptr) {
  if (ptr & 0x0000800000000000)
    return ptr | 0xffff000000000000;
  return ptr;
}

static void _mm_combine_forward(memseg_t *seg) {
  nullsafe(seg);
  nullsafe(seg->next);
  if ((uintptr_t)(seg->next) != ((uintptr_t)seg + (seg->pages << 12)))
    return;
  if (seg->next == _last)
    _last = seg;
  if (seg->next->next != NULL)
    seg->next->next->prev = seg;
  seg->pages += seg->next->pages;
  seg->next = seg->next->next;
}

static void _mm_combine_backward(memseg_t *seg) {
  nullsafe(seg);
  _mm_combine_forward(seg->prev);
}

bool mm_init(struct limine_memmap_response *mmap) {
  i32 usable = 0;
  for (u64 i = 0; i < mmap->entry_count; i++) {
    if (mmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
      usable++;
      free_page_block((void *)HHDM(mmap->entries[i]->base),
                      mmap->entries[i]->length >> 12);
    }
  }
  return usable != 0;
}

void *request_page() { return request_page_block(1); }

void *request_page_block(usz n) {
  if (!PHY(_first)) {
    kernel_panic_error("No memory");
    return NULL;
  }

  lock;

  memseg_t *s = _first;
  while (PHY(s)) {
    if (s->pages >= n) {
      if (s->pages > n) {
        memseg_t *ns = (memseg_t *)((uptr)s + (n << 12));
        ns->prev = s->prev;
        ns->next = s->next;
        ns->pages = s->pages - n;
        if (ns->prev)
          ns->prev->next = ns;
        if (ns->next)
          ns->next->prev = ns;
        if (s == _last)
          _last = ns;
        if (s == _first)
          _first = ns;
        unlock;
        return s;
      }
      if (s->prev)
        s->prev->next = s->next;
      if (s->next)
        s->next->prev = s->prev;
      if (s == _last)
        _last = s->prev;
      if (s == _first)
        _first = s->next;
      unlock;
      return s;
    }
    s = s->next;
  }
  kernel_panic_error("Out of memory");
  return NULL;
}

void free_page(void *p) { free_page_block(p, 1); }

void free_page_block(void *p, usz n) {
  nullsafe(p);
  nullsafe(n);
  lock;
  if (!_first) {
    _first = p;
    _first->next = NULL;
    _first->prev = NULL;
    _first->pages = n;
    unlock;
    return;
  }
  uptr is = (uptr)p;
  uptr ie = (uptr)p + (n << 12);
  memseg_t *np = NULL;
  memseg_t *nn = NULL;
  memseg_t *s = _first;
  while (s) {
    uptr ss = (uptr)s;
    uptr se = (uptr)s + (s->pages << 12);
    bool skip = false;
    if (is >= ss && ie <= se) {
      unlock;
      return; // Block is completely inside other block
    }
    if (is >= ss && is <= se) { // Block is partially inside other block
      is = se;
      if (ie <= is) {
        unlock;
        return;
      }
    }
    if (is <= ss) {
      if (ie >= se) { // Other block is inside block
        if (s->prev)
          s->prev->next = s->next;
        if (s->next)
          s->next->prev = s->prev;
        skip = true;
      } else { // Block partially inside other block
        ie = ss;
      }
    }
    if (!skip) {
      if (ie <= ss)
        nn = s;
      if (is >= se)
        np = s;
    }
    s = s->next;
  }
  memseg_t *ns = (memseg_t *)is;
  ns->pages = (ie - is) >> 12;
  ns->next = nn;
  ns->prev = np;
  if (nn)
    nn->prev = ns;
  if (np)
    np->next = ns;
  if (is > (uptr)_last)
    _last = ns;
  if (is < (uptr)_first)
    _first = ns;
  _mm_combine_forward(ns);
  _mm_combine_backward(ns);
  unlock;
}
