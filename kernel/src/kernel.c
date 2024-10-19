#include <kernel/config.h>
#include <kernel/debug.h>
#include <kernel/hw/acpi/acpi.h>
#include <kernel/hw/cpu/apic.h>
#include <kernel/hw/cpu/cpu.h>
#include <kernel/hw/devman/devman.h>
#include <kernel/hw/hid/kb/kb.h>
#include <kernel/initgraph.h>

long long main() {
  logf(LOGLEVEL_ALWAYS, "Starting limix v%u.%u.%u", KERNEL_MAJ, KERNEL_MIN,
       KERNEL_PATCH);

  INITGRAPH_STAGE("irq", NULL);
  INITGRAPH_STAGE("drivers", "irq");
  INITGRAPH_STAGE("enumeration", "drivers");

  initgraph_run("enumeration");

  return 0;
}
