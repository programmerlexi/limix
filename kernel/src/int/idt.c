#include "kernel/int/idt.h"
#include "kernel/mm/hhtp.h"
#include "kernel/mm/mm.h"

idt_gate_t *g_idt;

idt_desc_t IDTR;

void idt_init() {
  g_idt = request_page();
  isr_init();
  IDTR.addr = PHY((u64)g_idt);
  IDTR.size = 4095;
  asm("lidt %0" ::"m"(IDTR));
}
void idt_add_handler(u8 id, void *handler, u8 flags, u8 ist) {
  g_idt[id] = (idt_gate_t){.ist = ist,
                           .segment_selector = 0x28,
                           .flags = flags,
                           .offset_low = (u16)(u64)handler,
                           .offset_mid = (u16)((u64)handler >> 16),
                           .offset_high = (u32)((u64)handler >> 32),
                           .reserved = 0};
}
