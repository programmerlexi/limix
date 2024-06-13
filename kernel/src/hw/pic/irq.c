#include "kernel/hw/pic/pic.h"
#include "kernel/int/idt.h"
#include "libk/types.h"

void irq_0();
void irq_1();
void irq_2();
void irq_3();
void irq_4();
void irq_5();
void irq_6();
void irq_7();
void irq_8();
void irq_9();
void irq_10();
void irq_11();
void irq_12();
void irq_13();
void irq_14();
void irq_15();

static void (*handlers[16])();

void irq_handler(u64 irq) {
  if (pic_get_isr() & (1 << irq))
    return;
  if (handlers[irq])
    handlers[irq]();
  else // The handler is expected to end the interrupt
    pic_send_eoi(irq);
}
void(*irq_stub_table[16]) = {
    irq_0, irq_1, irq_2,  irq_3,  irq_4,  irq_5,  irq_6,  irq_7,
    irq_8, irq_9, irq_10, irq_11, irq_12, irq_13, irq_14, irq_15,
};

void irq_init() {
  asm("cli");
  for (i32 i = 0; i < 16; i++)
    idt_add_handler(
        0x20 + i, irq_stub_table[i],
        IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT, 0);
  asm("sti");
}
void irq_register(void (*handler)(), u8 irq) { handlers[irq] = handler; }
