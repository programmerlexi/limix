#pragma once

#include "kernel/hw/pci/pci.h"
#include "libk/types.h"

typedef struct {
  u32 command;
  u32 namespace_id;
  u64 reserved;
  u64 metadata_pointer;
  u64 data_pointer[2];
  u32 command_specific[6];
} __attribute__((packed)) nvme_submission_queue_entry_t;

typedef struct {
  u32 command_specific;
  u32 reserved;
  u16 submission_queue_head_pointer;
  u16 submission_queue_identifier;
  u16 command_identifier;
  bool phase : 1;
  u16 status_field : 15;
} __attribute__((packed)) nvme_completion_queue_entry_t;

typedef struct {
  u64 capabilities;
  u32 version;
  u32 interrupt_mask_set;
  u32 interrupt_mask_clear;
  u32 controller_configuration;
  u32 reserved0;
  u32 controller_status;
  u32 reserved1;
  u32 admin_queue_attributes;
  u64 admin_submission_queue;
  u64 admin_completion_queue;
  char reserved2[0xfc8];
} __attribute__((packed)) nvme_registers_t;

typedef struct {
  pci_type0_t *nvme_device;
  nvme_registers_t *bar0;
  nvme_submission_queue_entry_t *admin_submission_queue;
  nvme_completion_queue_entry_t *admin_completion_queue;
} nvme_t;

nvme_t *nvme_init(pci_type0_t *h);
void nvme_probe(nvme_t *nvme);
