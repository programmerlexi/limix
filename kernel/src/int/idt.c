#include "int/idt.h"
#include "defines.h"
#include "int/syscall.h"
#include "io/serial/serial.h"
#include "kernel.h"
#include "utils/memory/memory.h"
#include "utils/strings/strings.h"

idt_gate_t g_idt[256];
static inline void _lidt(void *base, u16 size) {
  struct {
    u16 length;
    void *base;
  } __attribute__((packed)) IDTR = {size, base};

  asm("lidt %0" : : "m"(IDTR)); // let the compiler choose an addressing mode
}

__attribute__((interrupt)) void gpf_handler(interrupt_frame_t *int_frame) {
  asm("cli");
  serial_writes("Encountered GPF at 0x");
  char b[16];
  ntos(b, int_frame->rip, 16, 16, TRUE, TRUE);
  serial_writes(b);
  serial_writes("\n\r");
  kernel_panic_error("General Protection Fault (Check serial)");
}
__attribute__((interrupt)) void df_handler(interrupt_frame_t *int_frame) {
  asm("cli");
  serial_writes("DF 0x");
  char b[16];
  ntos(b, int_frame->rip, 16, 16, TRUE, TRUE);
  serial_writes(b);
  serial_writes("\n\r");
  kernel_panic_error("Double Fault (Check serial)");
}

void idt_init() {
  kmemset(g_idt, 0, sizeof(idt_gate_t) * 256);

  idt_add_handler(0x0D, (void *)gpf_handler,
                  IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_TRAP,
                  0);
  idt_add_handler(0x08, (void *)df_handler,
                  IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_TRAP,
                  0);

  idt_add_handler(0x80, (void *)_syscall,
                  IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT,
                  0);

  _lidt((void *)g_idt, sizeof(g_idt) - 1);
  asm volatile("sti" ::: "memory");
}
void idt_add_handler(u8 id, void *handler, u8 flags, u8 ist) {
  idt_gate_t ig = {.offset_low = (u16)((u64)handler & 0xffff),
                   .segment_selector = 0x8,
                   .ist = ist,
                   .flags = flags,
                   .offset_mid = (u16)(((u64)handler & 0xffff0000) >> 16),
                   .offset_high = (u32)(((u64)handler & (~0xffffffff)) >> 32),
                   .reserved = 0};
  g_idt[id] = ig;
}
