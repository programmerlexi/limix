#include "fs/devfs.h"
#include <boot/limine.h>
#include <boot/requests.h>
#include <config.h>
#include <debug.h>
#include <fs/vfs.h>
#include <gdt/gdt.h>
#include <gfx/drm.h>
#include <gfx/font/font.h>
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <gfx/vt/vt.h>
#include <hw/hid/kb/kb.h>
#include <hw/hid/kb/poll.h>
#include <hw/pci/pci.h>
#include <hw/ps2.h>
#include <int/idt.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <kipc/semaphore.h>
#include <mm/heap.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <mm/vmm.h>
#include <smp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <task/async.h>
#include <task/sched/global.h>
#include <types.h>
#include <utils/strings/strings.h>

uint64_t hhaddr;

void _start(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  serial_early_init();
  serial_writes("Serial initialized!\n\r");

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1) {
    serial_writes("[!!] This kernel is meant for graphical systems. Please "
                  "attach a monitor!\n\r");
    hcf();
  }

  fb_init(framebuffer_request.response->framebuffers[0]);

  gdt_init();
  idt_init();

  if (paging_request.response->mode != USED_PAGING_MODE) {
    serial_writes("[!!] Paging mode isn't matching!\n\r");
    hcf();
  }

  hhaddr = hhdm_request.response->offset;

  if (!mm_init(mmap_request.response)) {
    serial_writes("[!!] MM seems to have failed\n\r");
    hcf();
  }

  init_kernel_vas();

  heap_init();

  drm_init();
  drm_switch(0);

  vt_init(0);

  font_parse();

  kprint("Welcome to " KERNEL_NAME " " KERNEL_RELEASE " " KERNEL_MAJ
         "." KERNEL_MIN "." KERNEL_PATCH "-" KERNEL_TYPE "\n\r");

  async_init();

  vfs_init();
  devfs_init();
  devfs_bind(vfs_make("dev"));
  devfs_reload();

  pci_init();
  /*ps2_init();
  kb_init();
  kb_init_polling();
  sched_glob_init();

  smp_init();

  while (1)
    sched_glob_tick();*/

  hcf();
}
