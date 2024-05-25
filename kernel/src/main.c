#include "kernel/boot/requests.h"
#include "kernel/config.h"
#include "kernel/debug.h"
#include "kernel/fs/devfs.h"
#include "kernel/fs/vfs.h"
#include "kernel/gdt/gdt.h"
#include "kernel/gfx/drm.h"
#include "kernel/gfx/font/font.h"
#include "kernel/gfx/framebuffer.h"
#include "kernel/hw/hid/kb/kb.h"
#include "kernel/hw/hid/kb/poll.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/ps2.h"
#include "kernel/int/idt.h"
#include "kernel/io/serial/serial.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "kernel/mm/vmm.h"
#include "kernel/smp.h"
#include "kernel/task/async.h"
#include "kernel/task/sched/global.h"
#include "libk/types.h"
#include "limine.h"
#include <stddef.h>

uptr g_hhaddr;

void _start(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  serial_early_init();
  serial_writes("Serial initialized!\n\r");

  if (g_framebuffer_request.response == NULL ||
      g_framebuffer_request.response->framebuffer_count < 1) {
    kernel_panic_error("This kernel is meant for graphical systems. Please "
                       "attach a monitor!");
  }

  fb_init(g_framebuffer_request.response->framebuffers[0]);

  if (g_paging_request.response->mode != USED_PAGING_MODE) {
    kernel_panic_error("Paging mode isn't matching!");
  }

  gdt_init();
  idt_init();

  g_hhaddr = g_hhdm_request.response->offset;

  if (!mm_init(g_mmap_request.response)) {
    kernel_panic_error("MM seems to have failed.");
  }

  init_kernel_vas();

  heap_init();

  config_init();

  drm_init();
  drm_switch(0);

  vt_init(0);

  font_parse();

  log(LOGLEVEL_INFO, "Welcome to limix " xstr(KERNEL_MAJ) "." xstr(
                         KERNEL_MIN) "." xstr(KERNEL_PATCH));

  async_init();

  vfs_init();

  devfs_init();
  devfs_bind(vfs_make("dev"));
  devfs_reload();

  drm_register_vfs();

  pci_init();
  ps2_init();
  kb_init();
  kb_init_polling();
  sched_glob_init();

  smp_init();

  while (1)
    sched_glob_tick();

  hcf();
}
