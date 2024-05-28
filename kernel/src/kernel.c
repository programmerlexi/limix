#include "kernel/debug.h"
#include "kernel/fs/devfs.h"
#include "kernel/fs/vfs.h"
#include "kernel/gfx/drm.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/hid/kb/poll.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/ps2.h"
#include "kernel/smp.h"
#include "kernel/task/sched/common.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"

static local_scheduler_t *ls;

void core_main() {
  log(LOGLEVEL_INFO, "Core main entered");
  for (;;)
    ;
}

long long main() {
  vfs_init();
  devfs_init();
  devfs_bind(vfs_make("dev"));
  devfs_reload();
  drm_register_vfs();
  pci_init();
  ps2_init();
  kb_init();
  kb_init_polling();

  u64 cpus = smp_init();

  for (u64 i = 0; i < cpus; i++)
    sched_create(core_main, i);

  ls = sched_local_init(0);

  // FIXME: Doesn't work, because process just keeps cpu forever.
  while (1) {
    sched_glob_tick();
    sched_local_tick(ls);
  }

  return 0;
}
