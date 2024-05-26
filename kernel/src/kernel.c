#include "kernel/fs/devfs.h"
#include "kernel/fs/vfs.h"
#include "kernel/gfx/drm.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/hid/kb/poll.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/ps2.h"
#include "kernel/smp.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"

static local_scheduler_t *ls;

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
  smp_init();

  ls = sched_local_init(0);

  while (1) {
    sched_glob_tick();
    sched_local_tick(ls);
  }

  return 0;
}
