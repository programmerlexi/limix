#include "kernel/hw/storage/ide/ide.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "ide"

Ide *ide_init(PciType0 *h) {
  Ide *ide = kmalloc(sizeof(*ide));
  ide->ide_device = h;
  ide->bar0 = h->bar0 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar1 = h->bar1 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar2 = h->bar2 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar3 = h->bar3 & PCI_BAR_MEM_BASE_ADDR;
  log(LOGLEVEL_INFO, "Initialized IDE");
  return ide;
}
