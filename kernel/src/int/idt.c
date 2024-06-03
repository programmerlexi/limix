#include "kernel/int/idt.h"
#include "kernel/int/syscall.h"
#include "libk/utils/memory/memory.h"

__attribute__((aligned(0x10))) idt_gate_t g_idt[256];

static idt_desc_t IDTR;

void idt_load() {
  asm("cli");
  asm("lidt %0" ::"m"(IDTR));
  asm("sti");
}

void idt_init() {
  kmemset(&g_idt[0], 0, sizeof(g_idt));
  isr_init();
  idt_add_handler(0x80, _syscall,
                  IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT,
                  0);
  IDTR.addr = (u64)&g_idt[0];
  IDTR.size = sizeof(g_idt) - 1;
  idt_load();
}
void idt_add_handler(u8 id, void *handler, u8 flags, u8 ist) {
  g_idt[id] = (idt_gate_t){.ist = ist,
                           .segment_selector = 0x08,
                           .flags = flags,
                           .offset_low = (u16)(u64)handler,
                           .offset_mid = (u16)((u64)handler >> 16),
                           .offset_high = (u32)((u64)handler >> 32),
                           .reserved = 0};
}
