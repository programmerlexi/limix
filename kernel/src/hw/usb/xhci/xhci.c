#include "kernel/hw/usb/xhci/xhci.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "xhci"

xhci_t *xhci_init(pci_type0_t *h) {
  xhci_t *xhci = kmalloc(sizeof(*xhci));
  xhci->xhci_device = h;
  uptr phy_addr = h->bar0 & PCI_BAR_MEM_BASE_ADDR;
  if ((h->bar0 & PCI_BAR_MEM_TYPE) == 2)
    phy_addr |= (u64)(h->bar1 & PCI_BAR_MEM_BASE_ADDR) << 32;
  xhci->cap = (void *)HHDM(phy_addr);
  log(LOGLEVEL_INFO, "Initialized XHCI");
  return xhci;
}
