#include <boot/limine.h>
#include <boot/requests.h>
#include <config.h>
#include <gfx/drm.h>
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <gfx/vt/vt.h>
#include <int/idt.h>
#include <io/serial/serial.h>
#include <kipc/semaphore.h>
#include <mm/hhtp.h>
#include <mm/mm.h>
#include <smp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <task/async.h>
#include <utils/strings/strings.h>

void hcf(void) {
  asm("cli");
  for (;;) {
    asm("hlt");
  }
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

  smp_init();
  async_init();

  drm_init();
  drm_sync();

  vt_init();

  kprint("Welcome to " KERNEL_NAME " " KERNEL_RELEASE " " KERNEL_MAJ
         "." KERNEL_MIN "." KERNEL_PATCH "-" KERNEL_TYPE "\n\r");

  hcf();
}
