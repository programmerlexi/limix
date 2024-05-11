#include "hw/pci/pci.h"
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "pci"

static void _pci_init_function(u8 b, u8 d, u8 f) {
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_VENDOR) == 0xffff)
    return;
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_DEVICE) == 0)
    return;
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_DEVICE) == 0xffff)
    return;
}
static void _pci_init_device(u8 b, u8 d) {
  u8 f;
  for (f = 0; f < 8; f++) {
    _pci_init_function(b, d, f);
  }
}
static void _pci_init_bus(u8 b) {
  u8 d;
  for (d = 0; d < 32; d++) {
    _pci_init_device(b, d);
  }
}

void pci_init() {
  u8 f;
  u8 b;
  u16 ht = pci_config_read_word(0, 0, 0, PCI_OFFSET_ALL_HEADER_TYPE);
  if ((ht & 0x80) == 0)
    _pci_init_bus(0);
  else
    for (f = 0; f < 8; f++) {
      if (pci_config_read_word(0, 0, f, PCI_OFFSET_ALL_VENDOR) != 0xffff)
        break;
      b = f;
      _pci_init_bus(b);
    }
}
