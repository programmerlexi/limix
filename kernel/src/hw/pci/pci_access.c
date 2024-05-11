#include "hw/pci/pci.h"
#include "io/pio.h"
#include <stdint.h>

u16 pci_config_read_word(u8 bus, u8 device, u8 func, u8 offset) {
  u32 addr;
  u32 lbus = (u32)bus;
  u32 lslot = (u32)device;
  u32 lfunc = (u32)func;
  u16 tmp = 0;

  addr = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) |
               ((u32)0x80000000));

  outd(PCI_CONFIG_ADDRESS, addr);
  u32 data = ind(PCI_CONFIG_DATA);
  u16 data015 = (u16)data;
  u16 data1531 = (u16)(data >> 16);
  data = (data015 << 16) | (data1531 << 0);
  tmp = (u16)((data >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}

u8 pci_config_read_byte(u8 bus, u8 slot, u8 func, u8 offset) {
  u16 w = pci_config_read_word(bus, slot, func, offset);
  return w & (0xff << 8 * (1 - (offset % 2))) >> 8 * (1 - (offset % 2));
  //  return (w >> ((~offset & 1) * 8)) & 0xff;
}
u16 pci_check_vendor(u8 bus, u8 slot) {
  return pci_config_read_word(bus, slot, 0, PCI_OFFSET_ALL_VENDOR);
}
