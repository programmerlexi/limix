#include "kernel/hw/ide/ide.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"

ide_t *ide_init(pci_type0_t *h) {
  ide_t *ide = kmalloc(sizeof(*ide));
  ide->ide_device = h;
  ide->bar0 = h->bar0;
  ide->bar1 = h->bar1;
  ide->bar2 = h->bar2;
  ide->bar3 = h->bar3;
  log(LOGLEVEL_INFO, "Initialized IDE");
  return ide;
}
