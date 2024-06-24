#include "kernel/kernel.h"
#include "kernel/asm_inline.h"
#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/cpu/apic.h"
#include "kernel/hw/cpu/cpu.h"
#include "kernel/hw/devman/devman.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/pcie/pcie.h"
#include "kernel/smp.h"
#include "kernel/task/sched/common.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"

static local_scheduler_t *ls;

void core_main() {
  cpu_init();
  logf(LOGLEVEL_INFO, "Running on a '%s'", cpu_vendor());

  apic_init();
}

void hardware_enumerate() {
  acpi_init();
  kb_init();
  if (!pcie_init())
    kernel_panic_error("PCIe init failed");
}

long long main() {
  logf(LOGLEVEL_ALWAYS, "Starting limix v%u.%u.%u", KERNEL_MAJ, KERNEL_MIN,
       KERNEL_PATCH);

  devman_init();

  sched_create(core_main, get_processor(), 0);
  sched_create(smp_init, get_processor(), 0);
  sched_create(hardware_enumerate, get_processor(), 0);

  ls = sched_local_init(get_processor());

  while (1) {
    sched_glob_tick();
    sched_local_tick(ls);
  }

  return 0;
}
