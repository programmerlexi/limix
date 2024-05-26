#include "kernel/int/idt.h"
#include "kernel/io/serial/serial.h"
#include "kernel/kernel.h"

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

void isr_init() {
  idt_add_handler(0, (void *)_isr0, 0x8e, 0);
  idt_add_handler(1, (void *)_isr1, 0x8e, 0);
  idt_add_handler(2, (void *)_isr2, 0x8e, 0);
  idt_add_handler(3, (void *)_isr3, 0x8e, 0);
  idt_add_handler(4, (void *)_isr4, 0x8e, 0);
  idt_add_handler(5, (void *)_isr5, 0x8e, 0);
  idt_add_handler(6, (void *)_isr6, 0x8e, 0);
  idt_add_handler(7, (void *)_isr7, 0x8e, 0);
  idt_add_handler(8, (void *)_isr8, 0x8e, 0);
  idt_add_handler(9, (void *)_isr9, 0x8e, 0);
  idt_add_handler(10, (void *)_isr10, 0x8e, 0);
  idt_add_handler(11, (void *)_isr11, 0x8e, 0);
  idt_add_handler(12, (void *)_isr12, 0x8e, 0);
  idt_add_handler(13, (void *)_isr13, 0x8e, 0);
  idt_add_handler(14, (void *)_isr14, 0x8e, 0);
  idt_add_handler(15, (void *)_isr15, 0x8e, 0);
  idt_add_handler(16, (void *)_isr16, 0x8e, 0);
  idt_add_handler(17, (void *)_isr17, 0x8e, 0);
  idt_add_handler(18, (void *)_isr18, 0x8e, 0);
  idt_add_handler(19, (void *)_isr19, 0x8e, 0);
  idt_add_handler(20, (void *)_isr20, 0x8e, 0);
  idt_add_handler(21, (void *)_isr21, 0x8e, 0);
  idt_add_handler(22, (void *)_isr22, 0x8e, 0);
  idt_add_handler(23, (void *)_isr23, 0x8e, 0);
  idt_add_handler(24, (void *)_isr24, 0x8e, 0);
  idt_add_handler(25, (void *)_isr25, 0x8e, 0);
  idt_add_handler(26, (void *)_isr26, 0x8e, 0);
  idt_add_handler(27, (void *)_isr27, 0x8e, 0);
  idt_add_handler(28, (void *)_isr28, 0x8e, 0);
  idt_add_handler(29, (void *)_isr29, 0x8e, 0);
  idt_add_handler(30, (void *)_isr30, 0x8e, 0);
  idt_add_handler(31, (void *)_isr31, 0x8e, 0);
}

static const char *exception_messages[] = {
    "Division By Zero Exeption.",
    "Debug Exeption.",
    "Non Maskable Interrupt Exeption.",
    "Breakpoint Exeption.",
    "Into Detected Overflow Exeption.",
    "Out of Bounds Exeption.",
    "Invalid Opcode Exeption.",
    "No Coprocessor Exeption.",
    "Double Fault Exeption.",
    "Coprocessor Segment Overrun Exeption.",
    "Bad TSS Exeption.",
    "Segment Not Present Exeption.",
    "Stack Fault Exeption.",
    "General Protection Fault Exeption.",
    "Page Fault Exeption.",
    "Unknown Interrupt Exeption.",
    "Coprocessor Fault Exeption.",
    "Alignment Check Exeption.",
    "Machine Check Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption.",
    "Reserved Exeption."};

void fault_handler(struct regs *r) {
  asm("cli");
  if (r->int_no < 32) {
    serial_writes((char *)exception_messages[r->int_no]);
    kernel_panic_error(exception_messages[r->int_no]);
  }
  hcf();
}
