#pragma once

#include <stdint.h>
#define IDT_FLAGS_GATE_TYPE_INT 0xe
#define IDT_FLAGS_GATE_TYPE_TRAP 0xf
#define IDT_FLAGS_DPL0 0x00
#define IDT_FLAGS_DPL1 0x20
#define IDT_FLAGS_DPL2 0x40
#define IDT_FLAGS_DPL3 0x60
#define IDT_FLAGS_PRESENT 0x80

typedef struct {
  uint16_t offset_low;
  uint16_t segment_selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t offset_mid;
  uint32_t offset_high;
  uint32_t reserved;
} __attribute__((packed)) idt_gate_t;
typedef struct {
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) idt_desc_t;
typedef struct {
  uint64_t err_code;
  uint64_t rip;
  uint32_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint32_t ss;
} __attribute__((packed)) interrupt_frame_t;
typedef struct {
  uint64_t rip;
  uint32_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint32_t ss;
} __attribute__((packed)) interrupt_frame_noerr_t;
void idt_init();
void idt_add_handler(uint8_t id, void *handler, uint8_t flags, uint8_t ist);
extern idt_gate_t idt[256];
