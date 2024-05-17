#include "boot/limine.h"
#include "boot/requests.h"
#include "config.h"
#include "config/config.h"
#include "debug.h"
#include "fs/devfs.h"
#include "fs/vfs.h"
#include "gdt/gdt.h"
#include "gfx/drm.h"
#include "gfx/font/font.h"
#include "gfx/framebuffer.h"
#include "gfx/vt/vt.h"
#include "hw/hid/kb/kb.h"
#include "hw/hid/kb/poll.h"
#include "hw/pci/pci.h"
#include "hw/ps2.h"
#include "int/idt.h"
#include "io/serial/serial.h"
#include "kernel.h"
#include "mm/heap.h"
#include "mm/hhtp.h"
#include "mm/mm.h"
#include "mm/vmm.h"
#include "smp.h"
#include "task/async.h"
#include "task/sched/global.h"
#include "types.h"
#include <stdbool.h>
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

  gdt_init();
  idt_init();

  if (g_paging_request.response->mode != USED_PAGING_MODE) {
    kernel_panic_error("Paging mode isn't matching!");
  }

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
