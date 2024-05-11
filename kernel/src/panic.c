#include "debug.h"
#include "gfx/vt/vt.h"
#include "io/serial/serial.h"
#include "kernel.h"

void hcf(void) {
  asm("cli");
  for (;;) {
    asm("hlt");
  }
}

void kernel_panic_error(const char *s) {
  serial_writes("ERROR: ");
  serial_writes((char *)s);
  serial_writes("\n\r");
  if (vt_is_available())
    logf(LOGLEVEL_FATAL, "[!!] %s", s);
  kernel_panic();
}
void kernel_panic() {
  serial_writes("An error has occured and the system will be halted.\n\r");
  if (vt_is_available())
    log(LOGLEVEL_FATAL, "An error has occured and the system will be halted.");
  hcf();
}
