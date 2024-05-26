#include "kernel/boot/requests.h"
#include "kernel/config.h"
#include "kernel/config/config.h"
#include "kernel/debug.h"
#include "kernel/gdt/gdt.h"
#include "kernel/gfx/drm.h"
#include "kernel/gfx/font/font.h"
#include "kernel/gfx/framebuffer.h"
#include "kernel/int/idt.h"
#include "kernel/io/serial/serial.h"
#include "kernel/kernel.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"
#include "kernel/mm/vmm.h"
#include "kernel/task/async.h"
#include "kernel/task/sched/global.h"
#include "limine.h"
#include <stddef.h>

uint64_t g_hhaddr;

long long main();

void _start() {
  if (LIMINE_BASE_REVISION_SUPPORTED == false)
    hcf();
  serial_early_init();
  if (g_framebuffer_request.response == NULL ||
      g_framebuffer_request.response->framebuffer_count < 1)
    kernel_panic_error("Could not find a framebuffer");
  if (g_paging_request.response->mode != USED_PAGING_MODE)
    kernel_panic_error("Paging mode doesn't match");
  g_hhaddr = g_hhdm_request.response->offset;
  gdt_init();
  idt_init();
  if (!mm_init(g_mmap_request.response))
    kernel_panic_error("MM init failed");
  init_kernel_vas();
  heap_init();
  config_init();
  fb_init(g_framebuffer_request.response->framebuffers[0]);
  drm_init();
  drm_switch(0);
  vt_init(0);
  font_parse();
  async_init();
  sched_glob_init();
  logf(LOGLEVEL_FATAL, "Kernel exit code: %i", main());
  log(LOGLEVEL_FATAL, "The kernel stopped executing (this should not happend)");
  hcf();
}
