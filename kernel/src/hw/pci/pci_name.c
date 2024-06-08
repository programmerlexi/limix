#include "kernel/hw/pci/pci.h"
#include "kernel/hw/pcie/pcie.h"
#include "libk/types.h"

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

static char *_get_vendor_name(u16 id) {
  switch (id) {
  case 0x8086:
    return "Intel Corp.";
  case 0x1022:
    return "AMD";
  case 0x10DE:
    return "NVIDIA Corporation";
  case 0x1B36:
    return "Red Hat, Inc.";
  case 0x10EC:
    return "Realtek Semiconductor Co., Ltd.";
  }
  return "-";
}

static const char *_get_device_name(u16 vID, u16 dID) {
  switch (vID) {
  case 0x8086: // Intel
    switch (dID) {
    case 0x29C0:
      return "Express DRAM Controller";
    case 0x2918:
      return "LPC Interface Controller";
    case 0x2922:
      return "6 port SATA Controller [AHCI mode]";
    case 0x2930:
      return "SMBus Controller";
    case 0x10d3:
      return "82574L Gigabit Network Connection";
    case 0x2934:
      return "82801I (ICH9 Family) USB UHCI Controller #1";
    case 0x2935:
      return "82801I (ICH9 Family) USB UHCI Controller #2";
    case 0x2936:
      return "82801I (ICH9 Family) USB UHCI Controller #3";
    case 0x293a:
      return "82801I (ICH9 Family) USB2 EHCI Controller #1";
    case 0x1910:
      return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor Host "
             "Bridge/DRAM Registers";
    case 0x1901:
      return "6th-10th Gen Core Processor PCIe Controller (x16)";
    case 0x1909:
      return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor PCIe "
             "Controller (x4)";
    case 0x191b:
      return "HD Graphics 530";
    case 0xa12f:
      return "100 Series/C230 Series Chipset Family USB 3.0 xHCI Controller";
    }
    break;
  case 0x1B36:     // Red Hat, Inc.
    switch (dID) { // QEMU Device range (Sponsored by Red Hat, Inc.)
    case 0x0001:
      return "QEMU PCI-PCI Bridge";
    case 0x0002:
      return "QEMU PCI 16550A Adapter";
    case 0x0003:
      return "QEMU PCI Dual-Port 16550A Adapter";
    case 0x0004:
      return "QEMU PCI Quad-Port 16550A Adapter";
    case 0x0005:
      return "QEMU PCI Test Device";
    case 0x0006:
      return "PCI Rocker Ethernet switch device";
    case 0x0007:
      return "PCI SD Card Host Controller Interface";
    case 0x0008:
      return "QEMU PCIe Host bridge";
    case 0x0009:
      return "QEMU PCI Expander bridge";
    case 0x000A:
      return "PCI-PCI bridge (multiseat)";
    case 0x000B:
      return "QEMU PCIe Expander bridge";
    case 0x000C:
      return "QEMU PCIe Root port";
    case 0x000D:
      return "QEMU XHCI Host Controller";
    case 0x0010:
      return "QEMU NVM Express Controller";
    case 0x0100:
      return "QXL paravirtual graphic card";
    }
    break;
  case 0x1234:
    switch (dID) {
    case 0x1111:
      return "QEMU stdvga";
    }
    break;
  case 0x10ec:
    switch (dID) {
    case 0x522a:
      return "RTS522A PCI Express Card Reader";
    }
    break;
  case 0x10de:
    switch (dID) {
    case 0x123d:
      return "GM108M [GeForce 940MX]";
    }
    break;
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
    return "ACCESS Bus";
  case 0x02:
    return "SSA";
  case 0x03:
    return "USB Controller";
  case 0x04:
    return "Fibre Channel";
  case 0x05:
    return "SMBus";
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
    pci_header_t *h = pcie_get_device(bus, slot, func);
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
    pci_header_t *h = pcie_get_device(bus, slot, func);
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
    pci_header_t *h = pcie_get_device(bus, slot, func);
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
    pci_header_t *h = pcie_get_device(bus, slot, func);
    if (!h)
      return "-";
    vendor = h->vendor_id;
  } else {
    vendor = pci_config_read_word(bus, slot, func, PCI_OFFSET_ALL_VENDOR);
  }
  return (char *)_get_vendor_name(vendor);
}
