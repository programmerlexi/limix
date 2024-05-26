#pragma once

#include "libk/types.h"

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
} __attribute__((packed)) idt_gate_t;

typedef struct {
  u16 size;
  u64 addr;
} __attribute__((packed)) idt_desc_t;

struct regs {
  u64 int_no, err_code;             /* our 'push byte #' and ecodes do this */
  u64 rip, cs, rflags, userrsp, ss; /* pushed by the processor automatically */
} __attribute__((packed));

void idt_init();
void idt_add_handler(u8 id, void *handler, u8 flags, u8 ist);
void isr_init();

extern idt_gate_t *g_idt;
