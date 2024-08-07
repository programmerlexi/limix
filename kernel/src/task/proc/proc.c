#include "kernel/task/proc/proc.h"
#include "kernel/asm_inline.h"
#include "kernel/mm/heap.h"

Process *proc_create() {
  Process *p = (Process *)kzalloc(sizeof(Process));
  return p;
}

void proc_destroy(Process *p) { kfree(p); }

void proc_switch(Process *p, Process *n) {
  p->cr3 = read_cr3();
  write_cr3(n->cr3);
}
