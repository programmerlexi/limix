#include "kernel/hw/usb/xhci/xhci.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include <stddef.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "xhci"

xhci_t *xhci_init(pci_type0_t *h) {
  xhci_t *xhci = kmalloc(sizeof(*xhci));
  xhci->xhci_device = h;
  logf(LOGLEVEL_DEBUG, "%l %l", (u64)h->bar0, (u64)h->bar1);
  uptr phy_addr = h->bar0 & PCI_BAR_MEM_BASE_ADDR;
  if ((h->bar0 & PCI_BAR_MEM_TYPE) >> 1 == 2)
    phy_addr |= (u64)(h->bar1 & PCI_BAR_MEM_BASE_ADDR) << 32;
  if (!phy_addr) {
    kfree(xhci);
    log(LOGLEVEL_WARN2, "XHCI driver received nullptr");
    return NULL;
  }
  xhci->cap = (void *)HHDM(phy_addr);
  logf(LOGLEVEL_INFO, "Initialized XHCI %l %u", xhci->cap,
       (u64)xhci->cap->cap_length);
  return xhci;
}
