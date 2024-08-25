#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/cpu/apic.h"
#include "kernel/hw/cpu/cpu.h"
#include "kernel/hw/devman/devman.h"
#include "kernel/hw/hid/kb/kb.h"

void core_main() {
  cpu_init();
  logf(LOGLEVEL_DEBUG, "Running on a '%s'", cpu_vendor());
  apic_init();
}

void hardware_enumerate() {
  acpi_init();
  kb_init();
  devman_enumerate();
}

long long main() {
  logf(LOGLEVEL_ALWAYS, "Starting limix v%u.%u.%u", KERNEL_MAJ, KERNEL_MIN,
       KERNEL_PATCH);

  core_main();

  devman_init();
  devman_add_drivers();

  hardware_enumerate();

  return 0;
}
