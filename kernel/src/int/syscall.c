#include <kernel/debug.h>
#include <kernel/int/idt.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "syscall"

void syscall(InterruptFrame *f) {
  switch (f->rax) {
  default:
    log(LOGLEVEL_WARN0, "Got unknown syscall");
    break;
  }
}
