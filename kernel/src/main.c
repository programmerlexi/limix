#include <kernel/boot/requests.h>
#include <kernel/config.h>
#include <kernel/config/config.h>
#include <kernel/debug.h>
#include <kernel/gdt/gdt.h>
#include <kernel/gfx/drm.h>
#include <kernel/gfx/font/font.h>
#include <kernel/gfx/framebuffer.h>
#include <kernel/gfx/vt/vt.h>
#include <kernel/hw/cpu/cpu.h>
#include <kernel/initgraph.h>
#include <kernel/int/idt.h>
#include <kernel/io/serial/serial.h>
#include <kernel/kernel.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/hhtp.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <libk/utils/memory/memory.h>
#include <limine.h>
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "init"

uint64_t g_hhaddr;
uint64_t g_virtual_base;
uint64_t g_physical_base;

extern u64 _bss_start;
extern u64 _bss_end;

extern void (*__init_array[])();
extern void (*__init_array_end[])();

long long main();

void _start() {
  if (LIMINE_BASE_REVISION_SUPPORTED == false)
    hcf();
  kmemset(&_bss_start, 0, (uptr)_bss_end - (uptr)_bss_start);
  serial_early_init();
  if (g_framebuffer_request.response == NULL ||
      g_framebuffer_request.response->framebuffer_count < 1)
    kernel_panic_error("Could not find a framebuffer");
  fb_init(g_framebuffer_request.response->framebuffers[0]);
  if (g_paging_request.response->mode != USED_PAGING_MODE)
    kernel_panic_error("Paging mode doesn't match");
  g_hhaddr = g_hhdm_request.response->offset;
  g_virtual_base = g_kernel_address_request.response->virtual_base;
  g_physical_base = g_kernel_address_request.response->physical_base;
  gdt_init();
  idt_init();
  pmm_init();
  heap_init();
  cpu_init();
  drm_init();
  drm_switch(0);
  vt_init(0);
  font_parse();
  initgraph_init();
  for (size_t i = 0; &__init_array[i] != __init_array_end; i++) {
    logf(LOGLEVEL_ANALYZE, "Running global constructor at 0x%x",
         __init_array[i]);
    __init_array[i]();
  }
  log(LOGLEVEL_DEBUG, "Global constructors done");
  logf(LOGLEVEL_FATAL, "Kernel exit code: %i", main());
  log(LOGLEVEL_FATAL, "The kernel stopped executing (this should not happend)");
  hcf();
}
