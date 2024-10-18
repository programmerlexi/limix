#pragma once

#include <libk/types.h>

#define IDT_FLAGS_GATE_TYPE_INT 0xe
#define IDT_FLAGS_GATE_TYPE_TRAP 0xf
#define IDT_FLAGS_DPL0 0x00
#define IDT_FLAGS_DPL1 0x20
#define IDT_FLAGS_DPL2 0x40
#define IDT_FLAGS_DPL3 0x60
#define IDT_FLAGS_PRESENT 0x80

typedef struct {
  u16 offset_low;
  u16 segment_selector;
  u8 ist;
  u8 flags;
  u16 offset_mid;
  u32 offset_high;
  u32 reserved;
} __attribute__((packed)) IdtGate;

typedef struct {
  u16 size;
  u64 addr;
} __attribute__((packed)) IdtDescriptor;

typedef struct {
  u64 r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx, rax, rbp,
      rip, cs, rflags, rsp;
} InterruptFrame;

void idt_init();
void idt_add_handler(u8 id, void *handler, u8 flags, u8 ist);
void isr_init();
void idt_load();

extern IdtGate g_idt[256];
