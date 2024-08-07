#include "kernel/hw/pci/pci.h"
#include "kernel/hw/pcie/pcie.h"

u16 pci_get_count(u16 vendor, u16 device) {
  u16 c = 0;
  for (u16 b = 0; b < 256; b++) {
    for (u8 s = 0; s < 32; s++) {
      for (u8 f = 0; f < 8; f++) {
        u16 v, d;
        if (pcie_initialized()) {
          PciHeader *h;
          h = pcie_get_device(b, s, f);
          v = h->vendor_id;
          d = h->device_id;
        } else {
          v = pci_config_read_word(b, s, f, PCI_OFFSET_ALL_VENDOR);
          d = pci_config_read_word(b, s, f, PCI_OFFSET_ALL_DEVICE);
        }
        if (v != vendor || d != device)
          continue;
        c++;
      }
    }
  }
  return c;
}
PciAddress pci_search(u16 vendor, u16 device, u16 number) {
  PciAddress a;
  a.exists = false;
  if (pci_get_count(vendor, device) < number)
    return a;
  a.exists = true;
  u16 c = 0;
  for (u16 b = 0; b < 256; b++) {
    for (u8 s = 0; s < 32; s++) {
      for (u8 f = 0; f < 8; f++) {
        u16 v, d;
        if (pcie_initialized()) {
          PciHeader *h;
          h = pcie_get_device(b, s, f);
          v = h->vendor_id;
          d = h->device_id;
        } else {
          v = pci_config_read_word(b, s, f, PCI_OFFSET_ALL_VENDOR);
          d = pci_config_read_word(b, s, f, PCI_OFFSET_ALL_DEVICE);
        }
        if (v != vendor || d != device)
          continue;
        if (c == number) {
          a.bus = b;
          a.slot = s;
          a.func = f;
          return a;
        }
        c++;
      }
    }
  }
  a.exists = false;
  return a;
}
