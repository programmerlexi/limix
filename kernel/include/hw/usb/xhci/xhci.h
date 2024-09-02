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
} XhciCapabilityRegister;

typedef struct {
  PciType0 *xhci_device;
  XhciCapabilityRegister *cap;
} Xhci;

bool xhci_init(PciType0 *h);
