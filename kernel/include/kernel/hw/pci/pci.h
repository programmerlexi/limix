#pragma once

#include <libk/types.h>
#include <stdbool.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_HEADER_TYPE_GENERAL 0
#define PCI_HEADER_TYPE_PCI_TO_PCI 1
#define PCI_HEADER_TYPE_PCI_TO_CARDBUS 2

#define PCI_OFFSET_ALL_DEVICE 0
#define PCI_OFFSET_ALL_VENDOR 2

#define PCI_OFFSET_ALL_STATUS 4
#define PCI_OFFSET_ALL_COMMAND 6

#define PCI_OFFSET_ALL_CLASS 9
#define PCI_OFFSET_ALL_SUBCLASS 8
#define PCI_OFFSET_ALL_PROG_IF 11
#define PCI_OFFSET_ALL_REVISION 10

#define PCI_OFFSET_ALL_BIST 13
#define PCI_OFFSET_ALL_HEADER_TYPE 12
#define PCI_OFFSET_ALL_LATENCY_TIMER 15
#define PCI_OFFSET_ALL_CACHE_LINE_SIZE 14

#define PCI_COMMAND_IO_SPACE_ENABLE 1
#define PCI_COMMAND_MEMORY_SPACE_ENABLE 2
#define PCI_COMMAND_BUS_MASTER 4
#define PCI_COMMAND_SPECIAL_CYCLES 8
#define PCI_COMMAND_WRITE_AND_INVALIDATE_ENABLE 16
#define PCI_COMMAND_VGA_PALETTE_SNOOP 32
#define PCI_COMMAND_PARITY_ERROR_RESPONSE 64
#define PCI_COMMAND_SERR_ENABLE 256
#define PCI_COMMAND_FAST_B2B 512
#define PCI_COMMAND_INT_DISABLE 1024

#define PCI_STATUS_INT_STATUS 8
#define PCI_STATUS_CAP_LIST 16
#define PCI_STATUS_66MHZ_CAPABLE 32
#define PCI_STATUS_FAST_B2B_CAAPBLE 128
#define PCI_STATUS_MDP_ERROR 256
#define PCI_STATUS_DEVSEL_TIMING(t) ((t & 3) << 9)
#define PCI_STATUS_SIG_TARGET_ABORT 2048
#define PCI_STATUS_RECV_TARGET_ABORT 4096
#define PCI_STATUS_RECV_MASTER_ABORT 8192
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR 1 << 14
#define PCI_STATUS_DETECTED_PARITY_ERROR 1 << 15

#define PCI_BAR_ALL_MEMSPACE 0
#define PCI_BAR_ALL_IOSPACE 1

#define PCI_BAR_MEM_TYPE 6
#define PCI_BAR_MEM_PREFETCH 8
#define PCI_BAR_MEM_BASE_ADDR ~15

#define PCI_BAR_IO_BASE_ADDR ~3

typedef u32 PciBar;

typedef struct {
  u16 vendor_id;
  u16 device_id;
  u16 command;
  u16 status;
  u8 revision;
  u8 prog_if;
  u8 subclass;
  u8 class_code;
  u8 cache_line_size;
  u8 latency_timer;
  u8 header_type;
  u8 bist;
} PciHeader;

typedef struct {
  PciHeader h;
  PciBar bar0;
  PciBar bar1;
  PciBar bar2;
  PciBar bar3;
  PciBar bar4;
  PciBar bar5;
  u32 card_bus_cis_pointer;
  u16 subsystem_vendor;
  u16 subsystem;
  u32 erom_base;
  u8 capabilities_pointer;
  u8 reserved[7];
  u8 interrupt_line;
  u8 interrupt_pin;
  u8 min_grant;
  u8 max_latency;
} PciType0;

typedef struct {
  u8 bus;
  u8 slot;
  u8 func;
  bool exists;
} PciAddress;

void pci_init();
u16 pci_config_read_word(u8 bus, u8 slot, u8 func, u8 offset);
u8 pci_config_read_byte(u8 bus, u8 slot, u8 func, u8 offset);
u16 pci_check_vendor(u8 bus, u8 slot);
char *pci_get_classname(u8 bus, u8 slot, u8 func);
char *pci_get_subclassname(u8 bus, u8 slot, u8 func);
char *pci_get_typename(u8 bus, u8 slot, u8 func);
char *pci_get_device_name(u8 bus, u8 slot, u8 func);
char *pci_get_vendor_name(u8 bus, u8 slot, u8 func);
u16 pci_get_count(u16 vendor, u16 device);
PciAddress pci_search(u16 vendor, u16 device, u16 number);
