#include "kernel/boot/requests.h"
#include "kernel/hw/pci/codes.h"
#include "kernel/hw/pci/pci.h"
#include "kernel/hw/pcie/pcie.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/hhtp.h"
#include "libk/types.h"
#include "libk/utils/memory/memory.h"
#include "libk/utils/strings/strings.h"
#include "limine.h"
#include <stddef.h>

static const char *_device_class_names[] = {
    "Unclassified",           "Mass Storage Controller",
    "Network Controller",     "Display Controller",
    "Multimedia Controller",  "Memory Controller",
    "Bridge Device",          "Simple Communication Controller",
    "Base System Peripheral", "Input Device Controller",
    "Docking Station",        "Processor",
    "Serial Bus Controller",  "Wireless Controller",
    "Intelligent Controller", "Satellite Communication Controller",
    "Encryption Controller",  "Signal Processing Controller",
    "Processing Accelerator", "Non Essential Instrumentation"};

struct pci_vendor_register {
  u16 vID;
  u16 size;
  char name[];
} __attribute__((packed));

static struct limine_file *pci_vendor_registry = NULL;
static bool found_vendor_registry = false;

static char *_get_vendor_name(u16 id) {
  if (!found_vendor_registry) {
    for (u64 i = 0; i < g_module_request.response->module_count; i++) {
      struct limine_file *mod = g_module_request.response->modules[i];
      if (kstrncmp("pci_vendors", mod->cmdline, 11)) {
        found_vendor_registry = true;
        pci_vendor_registry = mod;
        break;
      }
    }
    if (!found_vendor_registry)
      return "-";
  }
  struct pci_vendor_register *registry =
      (void *)HHDM(pci_vendor_registry->address);
  u64 offset = 0;
  while (offset < pci_vendor_registry->size) {
    if (registry->vID == id) {
      u16 strs = registry->size - 4;
      char *ns = kzalloc(strs + 1);
      kmemcpy(ns, &registry->name, strs);
      return ns;
    }
    offset += registry->size;
    registry = (void *)((uptr)registry + registry->size);
  }
  return "-";
}

struct pci_device_register {
  u16 vID;
  u16 dID;
  u16 size;
  char name[];
} __attribute__((packed));

static struct limine_file *pci_device_registry = NULL;
static bool found_device_registry = false;

static const char *_get_device_name(u16 vID, u16 dID) {
  if (!found_device_registry) {
    for (u64 i = 0; i < g_module_request.response->module_count; i++) {
      struct limine_file *mod = g_module_request.response->modules[i];
      if (kstrncmp("pci_devices", mod->cmdline, 11)) {
        found_device_registry = true;
        pci_device_registry = mod;
        break;
      }
    }
    if (!found_device_registry)
      return "-";
  }
  struct pci_device_register *registry =
      (void *)HHDM(pci_device_registry->address);
  u64 offset = 0;
  while (offset < pci_device_registry->size) {
    if (registry->vID == vID) {
      if (registry->dID == dID) {
        u16 strs = registry->size - 6;
        char *ns = kmalloc(strs + 1);
        kmemcpy(ns, &registry->name, strs);
        return ns;
      }
    }
    offset += registry->size;
    registry = (void *)((uptr)registry + registry->size);
  }
  return "-";
}

static const char *_mass_storage_subclass_name(u8 scID) {
  switch (scID) {
  case 0x00:
    return "SCSI Bus Controller";
  case 0x01:
    return "IDE Controller";
  case 0x02:
    return "Floppy Disk Controller";
  case 0x03:
    return "IPI Bus Controller";
  case 0x04:
    return "RAID Controller";
  case 0x05:
    return "ATA Controller";
  case 0x06:
    return "Serial ATA";
  case 0x07:
    return "Serial Attached SCSI";
  case 0x08:
    return "Non-Volatile Memory Controller";
  case 0x80:
    return "Other";
  }
  return "-";
}

static const char *_serial_bus_subclass_name(u8 scID) {
  switch (scID) {
  case 0x00:
    return "FireWire (IEEE 1394) Controller";
  case 0x01:
    return "ACCESS Bus Controller";
  case 0x02:
    return "SSA Controller";
  case 0x03:
    return "USB Controller";
  case 0x04:
    return "Fibre Channel";
  case 0x05:
    return "SMBus Controller";
  case 0x06:
    return "Infiniband";
  case 0x07:
    return "IPMI Interface";
  case 0x08:
    return "SERCOS Interface (IEC 61491)";
  case 0x09:
    return "CANbus";
  case 0x80:
    return "SerialBusController - Other";
  }
  return "-";
}

static const char *_bridge_dev_subclass_name(u8 scID) {
  switch (scID) {
  case 0x00:
    return "Host Bridge";
  case 0x01:
    return "ISA Bridge";
  case 0x02:
    return "EISA Bridge";
  case 0x03:
    return "MCA Bridge";
  case 0x04:
    return "PCI-to-PCI Bridge";
  case 0x05:
    return "PCMCIA Bridge";
  case 0x06:
    return "NuBus Bridge";
  case 0x07:
    return "CardBus Bridge";
  case 0x08:
    return "RACEway Bridge";
  case 0x09:
    return "PCI-to-PCI Bridge";
  case 0x0a:
    return "InfiniBand-to-PCI Host Bridge";
  case 0x80:
    return "Other";
  }
  return "-";
}

static const char *_network_subclass_name(u8 scID) {
  switch (scID) {
  case 0x00:
    return "Ethernet Controller";
  case 0x01:
    return "Token Ring Controller";
  case 0x02:
    return "FDDI Controller";
  case 0x03:
    return "ATM Controller";
  case 0x04:
    return "ISDN Controller";
  case 0x05:
    return "WorldFlip Controller";
  case 0x06:
    return "PICMG 2.14 Multi Computing Controller";
  case 0x07:
    return "Infiniband Controller";
  case 0x08:
    return "Fabric Controller";
  case 0x80:
    return "Other";
  }
  return "-";
}
static const char *_get_subclass_name(u8 cID, u8 scID) {
  switch (cID) {
  case 0x01:
    return _mass_storage_subclass_name(scID);
  case 0x02:
    return _network_subclass_name(scID);
  case 0x03:
    switch (scID) {
    case 0x00:
      return "VGA Compatible Controller";
    }
  case 0x06:
    return _bridge_dev_subclass_name(scID);
  case 0x0C:
    return _serial_bus_subclass_name(scID);
  }
  return "-";
}

static const char *_get_progif_name(u8 cID, u8 scID, u8 prgIF) {
  switch (cID) {
  case 0x01:
    switch (scID) {
    case 0x06:
      switch (prgIF) {
      case 0x00:
        return "Vendor Specific Interface";
      case 0x01:
        return "AHCI 1.0";
      case 0x02:
        return "Serial Storage Bus";
      }
    }
  case 0x03:
    switch (scID) {
    case 0x00:
      switch (prgIF) {
      case 0x00:
        return "VGA Controller";
      case 0x01:
        return "8514-Compatible Controller";
      }
    }
  case 0x0C:
    switch (scID) {
    case 0x03:
      switch (prgIF) {
      case 0x00:
        return "UHCI Controller";
      case 0x10:
        return "OHCI Controller";
      case 0x20:
        return "EHCI (USB2) Controller";
      case 0x30:
        return "XHCI (USB3) Controller";
      case 0x80:
        return "Unspecified";
      case 0xFE:
        return "USB Device (Not a Host Controller)";
      }
    }
  }
  return "-";
}

char *pci_get_classname(u8 bus, u8 slot, u8 func) {
  u8 c;
  if (pcie_initialized()) {
    PciHeader *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    c = h->class_code;
  } else {
    c = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_CLASS);
  }
  if (c < 0x40)
    return (char *)_device_class_names[c];
  return "-";
}
char *pci_get_subclassname(u8 bus, u8 slot, u8 func) {
  u8 c, sc;
  if (pcie_initialized()) {
    PciHeader *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    c = h->class_code;
    sc = h->subclass;
  } else {
    c = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_CLASS);
    sc = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_CLASS);
  }
  return (char *)_get_subclass_name(c, sc);
}
char *pci_get_device_name(u8 bus, u8 slot, u8 func) {
  u16 vendor, device;
  if (pcie_initialized()) {
    PciHeader *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    vendor = h->vendor_id;
    device = h->device_id;
  } else {
    vendor = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_VENDOR);
    device = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_DEVICE);
  }

  return (char *)_get_device_name(vendor, device);
}
char *pci_get_vendor_name(u8 bus, u8 slot, u8 func) {
  u16 vendor;
  if (pcie_initialized()) {
    PciHeader *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    vendor = h->vendor_id;
  } else {
    vendor = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_VENDOR);
  }
  return (char *)_get_vendor_name(vendor);
}

char *pci_get_typename(u8 bus, u8 slot, u8 func) {
  u8 c, sc, pi;
  if (pcie_initialized()) {
    PciHeader *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    c = h->class_code;
    sc = h->subclass;
    pi = h->prog_if;
  } else {
    c = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_CLASS);
    sc = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_CLASS);
    pi = pci_config_read_byte(bus, slot, func, PCI_OFFSET_ALL_PROG_IF);
  }
  switch (c) {
  case PCI_CLASS_MASS_STORAGE:
    switch (sc) {
    case PCI_SUBCLASS_MASS_STORAGE_SATA:
      static char *ms_sata_pis[] = {"SATA Controller [Vendor-specific]",
                                    "SATA Controller [AHCI]",
                                    "SATA Controller [Serial Storage Bus]"};
      return ms_sata_pis[pi];
      break;
    default:
      return "SATA Controller";
    }
    break;
  case PCI_CLASS_DISPLAY:
    switch (sc) {
    case PCI_SUBCLASS_DISPLAY_VGA:
      static char *disp_vga_pis[] = {"VGA Controller", "8514-compatible VGA"};
      return disp_vga_pis[pi];
    }
    return pci_get_subclassname(bus, slot, func);
    break;
  case PCI_CLASS_BUS:
    switch (sc) {
    case PCI_SUBCLASS_BUS_USB:
      static char *pis_usb[] = {"USB 1.0 (UHCI) Controller",
                                "USB 1.0 (OHCI) Controller",
                                "USB 2.0 Controller", "USB 3.0 Controller"};
      pi >>= 4;
      if (pi > 3)
        return (char *)_get_progif_name(c, sc, pi << 4);
      return pis_usb[pi];
    default:
      return (char *)_get_subclass_name(c, sc);
    }
  }
  return pci_get_classname(bus, slot, func);
}
