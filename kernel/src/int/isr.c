#include "kernel/int/idt.h"
#include "kernel/io/serial/serial.h"
#include "kernel/kernel.h"
#include "libk/utils/strings/strings.h"
#include <stdint.h>

static const char *exception_messages[] = {
    "Division Exception",
    "Debug",
    "Non-Maskable Interrupt Exception",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved (0x0f)",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved (0x16)",
    "Reserved (0x17)",
    "Reserved (0x18)",
    "Reserved (0x19)",
    "Reserved (0x1a)",
    "Reserved (0x1b)",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved (0x1f)",
};

void fault_handler(u64 int_no, u64 err_code, u64 rip, u64 rsp) {
  if (int_no < 32) {
    serial_writes((char *)exception_messages[int_no]);
    serial_writes(" 0x");
    char b[16];
    ntos(b, rip, 16, 16, true, true);
    serial_writes(b);
    serial_writes(" 0x");
    ntos(b, rsp, 16, 16, true, true);
    serial_writes(b);
    serial_writes("\n\r");
    serial_writes(" 0x");
    ntos(b, *(uint64_t *)(rsp + 8), 16, 16, true, true);
    serial_writes(b);
    serial_writes("\n\r");
    kernel_panic_error(exception_messages[int_no]);
  }
  kernel_panic_error("An exception has occured");
  hcf();
}

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();

void(*isr_stub_table[32]) = {
    isr_0,  isr_1,  isr_2,  isr_3,  isr_4,  isr_5,  isr_6,  isr_7,
    isr_8,  isr_9,  isr_10, isr_11, isr_12, isr_13, isr_14, isr_15,
    isr_16, isr_17, isr_18, isr_19, isr_20, isr_21, isr_22, isr_23,
    isr_24, isr_25, isr_26, isr_27, isr_28, isr_29, isr_30, isr_31,
};

void isr_init() {
  for (int i = 0; i < 32; i++)
    idt_add_handler(i, (void *)isr_stub_table[i], 0x8e, 0);
}
