#include "int/syscall.h"
#include <gfx/framebuffer.h>
#include <gfx/vga.h>
#include <int/idt.h>
#include <io/serial/serial.h>
#include <mm/hhtp.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

idt_gate_t idt[256];
static inline void lidt(void *base, uint16_t size) {
  struct {
    uint16_t length;
    void *base;
  } __attribute__((packed)) IDTR = {size, base};

  asm("lidt %0" : : "m"(IDTR)); // let the compiler choose an addressing mode
}

__attribute__((interrupt)) void gpf_handler(interrupt_frame_t *int_frame) {
  asm("cli");
  serial_writes("Encountered GPF at 0x");
  char b[16];
  ntos(b, int_frame->rip, 16, 16, true, true);
  serial_writes(b);
  serial_writes("\n\r");
  for (;;)
    ;
}
__attribute__((interrupt)) void df_handler(interrupt_frame_t *int_frame) {
  asm("cli");
  serial_writes("DF 0x");
  char b[16];
  ntos(b, int_frame->rip, 16, 16, true, true);
  serial_writes(b);
  serial_writes("\n\r");
  for (;;)
    ;
}

void idt_init() {
  memset(idt, 0, sizeof(idt_gate_t) * 256);

  idt_add_handler(0x0D, (void *)gpf_handler,
                  IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_TRAP,
                  0);
  idt_add_handler(0x08, (void *)df_handler,
                  IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_TRAP,
                  0);

  idt_add_handler(0x80, (void *)_syscall,
                  IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT,
                  0);

  lidt((void *)idt, sizeof(idt) - 1);
  asm volatile("sti" ::: "memory");
}
void idt_add_handler(uint8_t id, void *handler, uint8_t flags, uint8_t ist) {
  idt_gate_t ig = {
      .offset_low = (uint16_t)((uint64_t)handler & 0xffff),
      .segment_selector = 0x8,
      .ist = ist,
      .flags = flags,
      .offset_mid = (uint16_t)(((uint64_t)handler & 0xffff0000) >> 16),
      .offset_high = (uint32_t)(((uint64_t)handler & (~0xffffffff)) >> 32),
      .reserved = 0};
  idt[id] = ig;
}
