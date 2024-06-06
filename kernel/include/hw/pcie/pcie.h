#pragma once

#include "kernel/hw/pci/pci.h"
#include "libk/types.h"
#include <stdbool.h>

typedef struct pcie_region {
  u8 start;
  u8 end;
  u64 base;
} pcie_region_t;

bool pcie_init();
pci_header_t *pcie_get_device(u8 b, u8 s, u8 f);
bool pcie_initialized();
