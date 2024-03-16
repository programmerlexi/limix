#include <hw/pci/pci.h>
#include <io/pio.h>
#include <stdint.h>

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                              uint8_t offset) {
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  addr = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                    (offset & 0xFC) | ((uint32_t)0x80000000));

  outd(0xCF8, addr);
  tmp = (uint16_t)((ind(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}
uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func,
                             uint8_t offset) {
  uint16_t w = pci_config_read_word(bus, slot, func, offset);
  return w & (0xff << 8 * (1 - (offset % 2))) >> 8 * (1 - (offset % 2));
}
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot) {
  return pci_config_read_word(bus, slot, 0, 0);
}
