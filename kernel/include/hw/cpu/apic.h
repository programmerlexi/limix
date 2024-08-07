#pragma once

#include "libk/types.h"
#include <stdbool.h>

#define APIC_BASE_MSR 0x1b
#define APIC_BASE_MSR_BSP 0x100
#define APIC_MASE_MSR_ENABLE 0x800

#define APIC_REG_LAPIC_ID 0x20
#define APIC_REG_LAPIC_VERSION 0x30
#define APIC_REG_TPR 0x80
#define APIC_REG_APR 0x90
#define APIC_REG_PPR 0xa0
#define APIC_REG_EOI 0xb0
#define APIC_REG_RRD 0xc0
#define APIC_REG_LOGICAL_DESTINATION 0xd0
#define APIC_REG_DESTINATION_FORMAT 0xe0
#define APIC_REG_SPURIOUS_INTERRUPT_VECTOR 0xf0
#define APIC_REG_ISR0 0x100
#define APIC_REG_ISR1 0x110
#define APIC_REG_ISR2 0x120
#define APIC_REG_ISR3 0x130
#define APIC_REG_ISR4 0x140
#define APIC_REG_ISR5 0x150
#define APIC_REG_ISR6 0x160
#define APIC_REG_ISR7 0x170
#define APIC_REG_TMR0 0x180
#define APIC_REG_TMR1 0x190
#define APIC_REG_TMR2 0x1a0
#define APIC_REG_TMR3 0x1b0
#define APIC_REG_TMR4 0x1c0
#define APIC_REG_TMR5 0x1d0
#define APIC_REG_TMR6 0x1e0
#define APIC_REG_TMR7 0x1f0
#define APIC_REG_IRR0 0x200
#define APIC_REG_IRR1 0x210
#define APIC_REG_IRR2 0x220
#define APIC_REG_IRR3 0x230
#define APIC_REG_IRR4 0x240
#define APIC_REG_IRR5 0x250
#define APIC_REG_IRR6 0x260
#define APIC_REG_IRR7 0x270
#define APIC_REG_ERROR_STATUS 0x280
#define APIC_REG_LVT_CONNECTED_MACHINE_CHECK_INTERRUPT 0x2f0
#define APIC_REG_ICR0 0x300
#define APIC_REG_ICR1 0x310
#define APIC_REG_LVT_TIMER 0x320
#define APIC_REG_LVT_THERMAL_SENSOR 0x330
#define APIC_REG_LVT_PERFORMANCE_MONITORING_COUNTERS 0x340
#define APIC_REG_LVT_LINT0 0x350
#define APIC_REG_LVT_LINT1 0x360
#define APIC_REG_LVT_ERROR 0x370
#define APIC_REG_TIMER_INITIAL 0x380
#define APIC_REG_TIMER_CURRENT 0x390
#define APIC_REG_TIMER_DIVIDE_CONFIG 0x3e0

typedef struct {
  u8 vector_number;

  u8 delivery_mode : 3;
  u8 reserved0 : 1;
  u8 interrupt_pending : 1;
  u8 polarity : 1;
  u8 remote_irr : 1;
  u8 trigger_mode : 1;

  u16 mask : 1;
  u16 timer_mode : 3;
  u16 reserved1 : 12;
} __attribute__((packed)) APICLvt;

typedef struct {
  u8 int_vector;

  u8 delivery_mode : 3;
  u8 destination_mode : 1;
  u8 delivery_status : 1;
  u8 reserved0 : 1;
  u8 clear_for_init_level_deassert : 1;
  u8 set_for_init_level_deassert : 1;

  u32 reserved1 : 2;
  u32 destination : 2;
  u32 reserved2 : 28;
} __attribute__((packed)) APICIcr;

void apic_init();
bool apic_check();

uptr apic_get_base();
void apic_set_base(uptr base);

void apic_write_reg(u16 reg, u32 data0);
u32 apic_read_reg(u16 reg);
