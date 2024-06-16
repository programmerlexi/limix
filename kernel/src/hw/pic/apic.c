#include "kernel/hw/pic/apic.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/hw/pic/pic.h"
#include "kernel/kernel.h"
#include "kernel/mm/hhtp.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "lapic"

bool apic_check() {
  return true; // TODO: Perform an actual check
}

uptr apic_get_base() { return rdmsr(APIC_BASE_MSR) & ~0xfff; }
void apic_set_base(uptr base) {
  wrmsr(APIC_BASE_MSR, base | APIC_MASE_MSR_ENABLE);
}

void apic_write_reg(u16 reg, u32 data0) {
  *(u32 *)(HHDM(apic_get_base()) + reg) = data0;
}
u32 apic_read_reg(u16 reg) { return *(u32 *)(HHDM(apic_get_base()) + reg); }

void apic_init() {
  if (!apic_check())
    kernel_panic_error(
        "Something just went incredibly wrong (APIC is not supported)");
  if (rdmsr(APIC_BASE_MSR) & APIC_BASE_MSR_BSP) {
    pic_init();
    pic_disable();
  }

  apic_set_base(apic_get_base());

  apic_write_reg(APIC_REG_SPURIOUS_INTERRUPT_VECTOR,
                 apic_read_reg(APIC_REG_SPURIOUS_INTERRUPT_VECTOR) | 0x1ff);

  log(LOGLEVEL_INFO, "Initialized LAPIC");
}
