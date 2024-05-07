#include <io/serial/serial.h>
#include <kernel.h>

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
  kernel_panic();
}
void kernel_panic() {
  serial_writes("An error has occured and the system will be halted.\n\r");
  hcf();
}
