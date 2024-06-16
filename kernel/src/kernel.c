#include "kernel/asm_inline.h"
#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/fs/devfs.h"
#include "kernel/fs/vfs.h"
#include "kernel/gfx/drm.h"
#include "kernel/hw/acpi/acpi.h"
#include "kernel/hw/cpu/cpu.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/pic/apic.h"
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
  pci_init();
}

void activate_cpus() { smp_init(); }

void fs_init() {
  vfs_init();
  devfs_init();
  devfs_bind(vfs_make("dev"));
  devfs_reload();
  drm_register_vfs();
}

long long main() {
  logf(LOGLEVEL_ALWAYS, "Starting limix v%u.%u.%u", KERNEL_MAJ, KERNEL_MIN,
       KERNEL_PATCH);

  kb_init();

  sched_create(fs_init, get_processor(), 0);
  sched_create(activate_cpus, get_processor(), 0);
  sched_create(hardware_enumerate, get_processor(), 0);
  sched_create(core_main, get_processor(), 0);

  ls = sched_local_init(0);

  while (1) {
    sched_glob_tick();
    sched_local_tick(ls);
  }

  return 0;
}
