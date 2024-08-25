#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/gfx/vt/vt.h"
#include "kernel/io/serial/serial.h"
#include "kernel/kernel.h"
#include <stdint.h>

__attribute__((noreturn)) void hcf(void) {
  asm("cli");
  for (;;) {
    asm("hlt");
  }
}

__attribute__((noreturn)) void kernel_panic_error(const char *s) {
  asm("cli");
  serial_writes("ERROR: ");
  serial_writes((char *)s);
  serial_writes("\n\r");
  if (vt_is_available())
    logf(LOGLEVEL_FATAL, "[!!] %s", s);
  kernel_panic();
}

struct stackframe {
  struct stackframe *rbp;
  uint64_t rip;
} __attribute__((packed));

void stack_trace(unsigned int max_frames) {
  struct stackframe *stk = __builtin_frame_address(0);
  logf(LOGLEVEL_FATAL, "Stack trace (at 0x%x):", stk);
  unsigned int frame;
  for (frame = 0; stk && frame < max_frames; ++frame) {
    logf(LOGLEVEL_FATAL, "  0x%x", stk->rip);
    if (!stk->rip || !stk->rbp)
      break;
    stk = stk->rbp;
  }
  log(LOGLEVEL_FATAL, "-- End trace --");
}

void kernel_panic() {
  serial_writes("An error has occured and the system will be halted.\n\r");
  if (vt_is_available()) {
    log(LOGLEVEL_FATAL, "An error has occured and the system will be halted.");
    stack_trace(PANIC_TRACE_LENGTH);
  }
  hcf();
}
