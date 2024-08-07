#pragma once

#include "kernel/hw/pci/pci.h"
#include "libk/types.h"
#include <stdbool.h>

typedef struct PcieRegionStruct {
  u8 start;
  u8 end;
  u64 base;
} PcieRegion;

bool pcie_init();
PciHeader *pcie_get_device(u8 b, u8 s, u8 f);
bool pcie_initialized();
