#include "mm/vmm.h"
#include <boot/limine.h>
#include <boot/requests.h>
#include <config.h>
#include <debug.h>
#include <gfx/drm.h>
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <gfx/vt/vt.h>
#include <hw/ps2.h>
#include <int/idt.h>
#include <io/serial/serial.h>
#include <kernel.h>
#include <kipc/semaphore.h>
#include <mm/heap.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <smp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <task/async.h>
#include <task/sched/global.h>
#include <types.h>
#include <utils/strings/strings.h>

void hcf(void) {
  asm("cli");
  for (;;) {
    asm("hlt");
  }
}

void kernel_panic_error(const char *s) {
  serial_writes("ERROR: ");
  serial_writes((char *)s);
  serial_writes("\n\r");
  kernel_panic();
}
void kernel_panic() {
  serial_writes("An error has occured and the system will be halted.\n\r");
  hcf();
}

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

  idt_init();

  if (paging_request.response->mode != USED_PAGING_MODE) {
    putstr16(0, 0, "Paging mode doesn't match! ABORTING!", 0xff0000);
    serial_writes("[!!] Paging mode isn't matching!\n\r");
    hcf();
  }

  hhaddr = hhdm_request.response->offset;

  if (!mm_init(mmap_request.response)) {
    putstr16(0, 0, "MM init failed! ABORTING!", 0xff0000);
    serial_writes("[!!] MM seems to have failed\n\r");
    hcf();
  }

  init_kernel_vas();

  heap_init();

  drm_init();
  drm_switch(0);

  vt_init(0);

  kprint("Welcome to " KERNEL_NAME " " KERNEL_RELEASE " " KERNEL_MAJ
         "." KERNEL_MIN "." KERNEL_PATCH "-" KERNEL_TYPE "\n\r");

  async_init();
  ps2_init();
  sched_glob_init();
  smp_init();

  debug("Waiting");
  for (int i = 0; i < 0xfffffff; i++) // Should be enough
    asm("nop");
  debug("Starting kernel loop");

  while (1)
    sched_glob_tick();

  hcf();
}
