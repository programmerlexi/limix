#include <kernel/mm/heap.h>
#include <libk/printing.h>

static usz _get_unit_index(usz v) {
  for (usz ui = 1; ui < 4; ui++) {
    if (!(v >> ((10 * ui) + 2)))
      return ui - 1;
  }
  return 3;
}

static char *_get_unit(usz v) {
  static char *units[] = {"B", "kB", "MB", "GB"};
  return units[_get_unit_index(v)];
}

void heap_print_usage() {
  usz used = heap_get_used();
  usz free = heap_get_free();
  usz overhead = heap_get_overhead();
  kprintf("Heap: F: %u %s U: %u %s (Overhead: %u %s)\n\r",
          free >> (10 * _get_unit_index(free)), _get_unit(free),
          used >> (10 * _get_unit_index(used)), _get_unit(used),
          overhead >> (10 * _get_unit_index(overhead)), _get_unit(overhead));
}
