#include "kernel/int/idt.h"
#include "kernel/asm_inline.h"
#include "kernel/debug.h"
#include "kernel/int/syscall.h"
#include "kernel/panic.h"
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
  panic_init();
  idt_add_handler(0x80, _syscall,
                  IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT,
                  0);
  IDTR.addr = (u64)&g_idt[0];
  IDTR.size = sizeof(g_idt) - 1;
  idt_load();
}

extern void _panic();

void panic_init() {
  idt_add_handler(0x40, _panic,
                  IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT | IDT_FLAGS_GATE_TYPE_INT,
                  0);
}

void panic_handle(int_frame_t *f) {
  logf(LOGLEVEL_FATAL, "RAX: 0x%l RBX: 0x%l", f->rax, f->rbx);
  logf(LOGLEVEL_FATAL, "RCX: 0x%l RDX: 0x%l", f->rcx, f->rdx);
  logf(LOGLEVEL_FATAL, "R8:  0x%l R9:  0x%l", f->r8, f->r9);
  logf(LOGLEVEL_FATAL, "R10: 0x%l R11: 0x%l", f->r10, f->r11);
  logf(LOGLEVEL_FATAL, "R12: 0x%l R13: 0x%l", f->r12, f->r13);
  logf(LOGLEVEL_FATAL, "R14: 0x%l R15: 0x%l", f->r14, f->r15);
  logf(LOGLEVEL_FATAL, "RDI: 0x%l RSI: 0x%l", f->rdi, f->rsi);
  logf(LOGLEVEL_FATAL, "RFLAGS: 0x%l", f->rflags);
  logf(LOGLEVEL_FATAL, "CR0: 0x%l CR2: 0x%l", read_cr0(), read_cr2());
  logf(LOGLEVEL_FATAL, "CR3: 0x%l CR4: 0x%l", read_cr3(), read_cr4());
  logf(LOGLEVEL_FATAL, "EFER: 0x%l", rdmsr(0xc0000080));
  logf(LOGLEVEL_FATAL, "RSP: 0x%l RBP: 0x%l", f->rsp, f->rbp);
  logf(LOGLEVEL_FATAL, "CS:  0x%l RIP: 0x%l", f->cs, f->rip);
  log(LOGLEVEL_FATAL, "kernel panic -- halting!");
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
