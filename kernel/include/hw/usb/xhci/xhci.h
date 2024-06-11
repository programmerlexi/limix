#pragma once

#include "kernel/hw/pci/pci.h"

typedef struct {
  pci_type0_t *xhci_device;
} xhci_t;

xhci_t *xhci_init(pci_type0_t *h);
