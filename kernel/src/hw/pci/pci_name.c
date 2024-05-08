#include "hw/pci/codes.h"
#include <hw/pci/pci.h>
#include <stdint.h>

char *pci_get_classname(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t c = pci_config_read_byte(bus, slot, func, PCI_OFFSET_ALL_CLASS);
  switch (c) {
  case 0:
    return "Unclassified";
  case 1:
    return "Mass Storage Controller";
  case 2:
    return "Network Controller";
  case 3:
    return "Display Controller";
  case 4:
    return "Mutimedia Controller";
  case 5:
    return "Memory Controller";
  case 6:
    return "Bridge";
  case 7:
    return "Simple Communication Controller";
  case 8:
    return "Base System Peripheral";
  case 9:
    return "Input Device Controller";
  case 10:
    return "Docking Station";
  case 11:
    return "Processor";
  case 12:
    return "Serial Bus Controller";
  case 13:
    return "Wireless Controller";
  case 14:
    return "Intelligent Controller";
  case 15:
    return "Satellite Communication Controller";
  case 16:
    return "Encryption Controller";
  case 17:
    return "Signal Processing Controller";
  case 18:
    return "Processing Accelerator";
  case 19:
    return "Non-Essential Instrumentation";
  case 64:
    return "Co-Processor";
  default:
    return "Unknown";
  }
}
char *pci_get_subclassname(uint8_t bus, uint8_t slot, uint8_t func);
