#pragma once

#include <libk/types.h>

typedef struct GdtDescriptorStruct {
  u16 size;
  u64 offset;
} __attribute__((packed)) GdtDescriptor;

typedef struct GdtEntryStruct {
  u16 limit_low;
  u16 base_low;
  u8 base_mid;
  u8 access;
  u8 flags_limit_hi;
  u8 base_hi;
} __attribute__((packed)) GdtEntry;

typedef struct GdtStruct {
  GdtEntry null;        // 0x00
  GdtEntry kernel_code; // 0x08
  GdtEntry kernel_data; // 0x10
  GdtEntry user_null;
  GdtEntry user_code;
  GdtEntry user_data;
} __attribute__((packed)) __attribute((aligned(0x1000))) Gdt;

typedef enum {
  GDT_ACCESS_CODE_READABLE = 0x02,
  GDT_ACCESS_DATA_WRITEABLE = 0x02,

  GDT_ACCESS_CODE_CONFORMING = 0x04,
  GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
  GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,

  GDT_ACCESS_DATA_SEGMENT = 0x10,
  GDT_ACCESS_CODE_SEGMENT = 0x18,

  GDT_ACCESS_DESCRIPTOR_LDT = 0x00,
  GDT_ACCESS_DESCRIPTOR_TSS = 0x01,

  GDT_ACCESS_RING0 = 0x00,
  GDT_ACCESS_RING1 = 0x20,
  GDT_ACCESS_RING2 = 0x40,
  GDT_ACCESS_RING3 = 0x60,

  GDT_ACCESS_PRESENT = 0x80,

} GdtAccess;

typedef enum {
  GDT_FLAG_64BIT = 0x20,
  GDT_FLAG_32BIT = 0x40,
  GDT_FLAG_16BIT = 0x00,

  GDT_FLAG_GRANULARITY_1B = 0x00,
  GDT_FLAG_GRANULARITY_4K = 0x80,
} GDTFlags;

extern Gdt g_gdt;

extern void load_gdt(GdtDescriptor *gdt_descriptor);
void gdt_init(void);
