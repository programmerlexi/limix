#include "kernel/task/proc/proc.h"
#include "kernel/asm_inline.h"
#include "kernel/mm/heap.h"

process_t *proc_create() {
  process_t *p = (process_t *)kmalloc(sizeof(process_t));
  return p;
}

void proc_destroy(process_t *p) { kfree(p); }

void proc_switch(process_t *p, process_t *n) {
  p->cr4 = read_cr4();
  write_cr4(n->cr4);
}
