#pragma once

#include "kernel/hw/pci/pci.h"

typedef struct {
  u8 cap_length;
  u8 reserved0;
  u16 interface_version;
  u32 structural_parameters0;
  u32 structural_parameters1;
  u32 structural_parameters2;
  u32 cap_params0;
  u32 doorbell_offset;
  u32 runtime_register_space_offset;
  u32 cap_params1;
} xhci_capability_register_t;

typedef struct {
  pci_type0_t *xhci_device;
  xhci_capability_register_t *cap;
} xhci_t;

xhci_t *xhci_init(pci_type0_t *h);
