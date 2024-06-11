#include "kernel/hw/usb/xhci/xhci.h"
#include "kernel/debug.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/mm/heap.h"

#undef DEBUG_MODULE
#define DEBUG_MODULE "xhci"

xhci_t *xhci_init(pci_type0_t *h) {
  xhci_t *xhci = kmalloc(sizeof(*xhci));
  xhci->xhci_device = h;
  log(LOGLEVEL_INFO, "Initialized XHCI");
  return xhci;
}
