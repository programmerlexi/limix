#include "kernel/task/proc/proc.h"
#include "kernel/asm_inline.h"
#include "kernel/mm/heap.h"

process_t *proc_create() {
  process_t *p = (process_t *)kcalloc(sizeof(process_t));
  return p;
}

void proc_destroy(process_t *p) { kfree(p); }

void proc_switch(process_t *p, process_t *n) {
  /*p->cr3 = read_cr3();*/
  /*write_cr3(n->cr3);*/
}
