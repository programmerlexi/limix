#include <kernel/asm_inline.h>
#include <kernel/boot/requests.h>
#include <kernel/debug.h>
#include <kernel/gdt/gdt.h>
#include <kernel/int/idt.h>
#include <kernel/io/pio.h>
#include <kernel/smp.h>
#include <limine.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "smp"

void core_main();

void __attribute__((noreturn)) _smp_start(struct limine_smp_info *cpu_info) {
  logf(LOGLEVEL_DEBUG, "[CPU %u] Received startup", get_processor());
  gdt_init();
  idt_load();
  for (;;)
    asm("hlt");
}

u64 smp_init() {
  for (u64 i = 0; i < g_smp_request.response->cpu_count; i++) {
    if (g_smp_request.response->bsp_lapic_id ==
        g_smp_request.response->cpus[i]->lapic_id)
      continue;
    log(LOGLEVEL_ANALYZE, "Setting entry");
    io_wait();
    g_smp_request.response->cpus[i]->extra_argument = 0;
    g_smp_request.response->cpus[i]->goto_address = _smp_start;
  }

  debug("All processors started");

  return g_smp_request.response->cpu_count;
}
