#include <asm_inline.h>
#include <mm/heap.h>
#include <task/proc/proc.h>

process_t *proc_create() {
  process_t *p = (process_t *)malloc(sizeof(process_t));
  return p;
}

void proc_destroy(process_t *p) { free(p); }

void proc_switch(process_t *p, process_t *n) {
  p->cr4 = read_cr4();
  write_cr4(n->cr4);
}
