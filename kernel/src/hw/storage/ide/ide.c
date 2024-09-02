#include "kernel/hw/storage/ide/ide.h"
#include "kernel/debug.h"
#include "kernel/hw/devman/devman.h"
#include "kernel/hw/pci/codes.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "ide"

static void dm_register_ide() {
  devman_register_driver(CLASSSUBCLASS, PCI_CLASS_MASS_STORAGE,
                         PCI_SUBCLASS_MASS_STORAGE_IDE, 0, ide_init);
}
__attribute__((used, section(".devman_construct"))) static void *reg =
    dm_register_ide;

bool ide_init(PciType0 *h) {
  Ide *ide = kmalloc(sizeof(*ide));
  ide->ide_device = h;
  ide->bar0 = h->bar0 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar1 = h->bar1 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar2 = h->bar2 & PCI_BAR_MEM_BASE_ADDR;
  ide->bar3 = h->bar3 & PCI_BAR_MEM_BASE_ADDR;
  log(LOGLEVEL_INFO, "Initialized IDE");
  return true;
}
