#include "gfx/vt/ansi.h"
#include "gfx/vt/vt.h"
#include <config.h>
#include <kernel.h>
#include <mm/heap.h>
#include <mm/mm.h>
#include <utils/memory/memory.h>

heapseg_t *heap_first;
heapseg_t *heap_last;

void heap_init() {
  heap_first = request_page_block(CONFIG_HEAP_INITIAL_PAGES);
  if (!heap_first)
    kernel_panic_error("Not enough memory for heap");
  heap_last = heap_first;
  memset(heap_first, 0, CONFIG_HEAP_INITIAL_PAGES * 0x1000);
  heap_first->next = NULL;
  heap_first->prev = NULL;
  heap_first->size = CONFIG_HEAP_INITIAL_PAGES * 0x1000 - sizeof(heapseg_t);
  heap_first->used = false;
  kprint(TERMCODE(TC_FG_GREEN)
             TERMCODE(TC_BOLD) "[+]" TERMCODE(TC_RESET) " Heap\n\r");
}
