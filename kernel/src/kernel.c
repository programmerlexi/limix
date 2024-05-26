#include "kernel/kernel.h"
#include "kernel/debug.h"
#include "kernel/fs/devfs.h"
#include "kernel/fs/vfs.h"
#include "kernel/gfx/drm.h"
#include "kernel/gfx/vt/vt.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/hid/kb/poll.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/ps2.h"
#include "kernel/smp.h"
#include "kernel/task/sched/global.h"
#include "kernel/task/sched/local.h"

static local_scheduler_t *ls;
void unlock_lschedi();

void a(void) {
  while (1) {
    kprintc('A');
    sched_local_tick(ls);
  }
}
void b(void) {
  while (1) {
    kprintc('B');
    sched_local_tick(ls);
  }
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
  smp_init();

  log(LOGLEVEL_INFO, "Creating processes");
  sched_create(a);
  sched_create(b);

  log(LOGLEVEL_INFO, "Running scheduler");
  log(LOGLEVEL_ANALYZE, "Unlocking scheduler");

  unlock_lschedi();
  log(LOGLEVEL_ANALYZE, "Creating scheduler");
  ls = sched_local_init(0);

  sched_glob_list_processes();

  sched_glob_tick();
  sched_local_tick(ls);

  return 0;
}
