#pragma once

#include "libk/types.h"
#include <stdbool.h>

typedef struct acpi_rsdp {
  char signature[8];
  u8 checksum;
  char oem[6];
  u8 revision;
  u32 rsdt_address;
  u32 length;

  u64 xsdt_address;
  u8 extended_checksum;
  u8 reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_gas {
  u8 addr_space;
  u8 bit_width;
  u8 bit_offset;
  u8 access_size;
  u64 addr;
} acpi_gas_t;

typedef struct acpi_table {
  char signature[4];
  u32 length;
  u8 revision;
  u8 checksum;
  char oem[6];
  char oem_table[8];
  u32 oem_revision;
  u32 creator_id;
  u32 creator_revision;
} __attribute__((packed)) acpi_table_t;

typedef struct acpi_xsdt {
  acpi_table_t header;
  u64 other_sdt[];
} __attribute__((packed)) acpi_xsdt_t;

typedef struct acpi_rsdt {
  acpi_table_t header;
  u32 other_sdt[];
} __attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_fadt {
  acpi_table_t header;
  u32 fw_ctl;
  u32 dsdt;
  u8 reserved0;
  u8 preferred_power_management;
  u16 sci_int;
  u32 smi_cmd_port;
  u8 acpi_enable;
  u8 acpi_disable;
  u8 s4bios_req;
  u8 pstate_ctl;
  u32 pm1a_event_block;
  u32 pm1b_event_block;
  u32 pm1a_control_block;
  u32 pm1b_control_block;
  u32 pm2_control_block;
  u32 pm_timer_block;
  u32 gpe0_block;
  u32 gpe1_block;
  u8 pm1_event_length;
  u8 pm1_control_length;
  u8 pm2_control_length;
  u8 pm_timer_length;
  u8 gpe0_length;
  u8 gpe1_length;
  u8 gpe1_base;
  u8 cstate_ctl;
  u16 worst_c2_latency;
  u16 worst_c3_latency;
  u16 flush_size;
  u16 flush_stride;
  u8 duty_offset;
  u8 duty_width;
  u8 day_alarm;
  u8 month_alarm;
  u8 century;

  // reserved in ACPI 1.0; used since ACPI 2.0+
  u16 boot_arch_flags;

  u8 reserved1;
  u32 flags;

  // 12 byte structure; see below for details
  acpi_gas_t reset_reg;

  u8 reset_value;
  u8 reserved2[3];

  // 64bit pointers - Available on ACPI 2.0+
  u64 x_firmware_ctl;
  u64 x_dsdt;

  acpi_gas_t x_pm1a_event_block;
  acpi_gas_t x_pm1b_event_block;
  acpi_gas_t x_pm1a_control_block;
  acpi_gas_t x_pm1b_control_block;
  acpi_gas_t x_pm2_control_block;
  acpi_gas_t x_pm_timer_block;
  acpi_gas_t x_gpe0_block;
  acpi_gas_t x_gpe1_block;
} __attribute__((packed)) acpi_fadt_t;

void acpi_init();
acpi_table_t *acpi_get(char *id);
bool acpi_check_table(acpi_table_t *table);
