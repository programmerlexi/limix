#include <debug.h>
#include <hw/pci/pci.h>
#include <stdint.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "pci"

static void pci_init_function(uint8_t b, uint8_t d, uint8_t f) {
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_VENDOR) == 0xffff)
    return;
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_DEVICE) == 0)
    return;
  if (pci_config_read_word(b, d, f, PCI_OFFSET_ALL_DEVICE) == 0xffff)
    return;
}
static void pci_init_device(uint8_t b, uint8_t d) {
  uint8_t f;
  for (f = 0; f < 8; f++) {
    pci_init_function(b, d, f);
  }
}
static void pci_init_bus(uint8_t b) {
  uint8_t d;
  for (d = 0; d < 32; d++) {
    pci_init_device(b, d);
  }
}

void pci_init() {
  uint8_t f;
  uint8_t b;
  uint16_t ht = pci_config_read_word(0, 0, 0, PCI_OFFSET_ALL_HEADER_TYPE);
  if ((ht & 0x80) == 0)
    pci_init_bus(0);
  else
    for (f = 0; f < 8; f++) {
      if (pci_config_read_word(0, 0, f, PCI_OFFSET_ALL_VENDOR) != 0xffff)
        break;
      b = f;
      pci_init_bus(b);
    }
}
