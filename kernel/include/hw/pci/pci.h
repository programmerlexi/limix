#pragma once

#include <hw/pci/codes.h>
#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_HEADER_TYPE_GENERAL 0
#define PCI_HEADER_TYPE_PCI_TO_PCI 1
#define PCI_HEADER_TYPE_PCI_TO_CARDBUS 2

#define PCI_OFFSET_ALL_VENDOR 0
#define PCI_OFFSET_ALL_DEVICE 2
#define PCI_OFFSET_ALL_COMMAND 4
#define PCI_OFFSET_ALL_STATUS 6
#define PCI_OFFSET_ALL_REVISION 8
#define PCI_OFFSET_ALL_PROG_IF 9
#define PCI_OFFSET_ALL_SUBCLASS 10
#define PCI_OFFSET_ALL_CLASS 11
#define PCI_OFFSET_ALL_CACHE_LINE_SIZE 12
#define PCI_OFFSET_ALL_LATENCY_TIMER 13
#define PCI_OFFSET_ALL_HEADER_TYPE 14
#define PCI_OFFSET_ALL_BIST 15

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

typedef struct {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command;
  uint16_t status;
  uint8_t revision;
  uint8_t prog_if;
  uint8_t subclass;
  uint8_t class_code;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;
} pci_header_t;

void pci_init();
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                              uint8_t offset);
uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func,
                             uint8_t offset);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot);
