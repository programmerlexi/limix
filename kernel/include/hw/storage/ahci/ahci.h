#pragma once

#include "kernel/hw/pci/pci.h"
#include "libk/types.h"

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000
#define HBA_PxIS_TFES (1 << 30)

#define AHCI_FIS_TYPE_REG_H2D 0x27
#define AHCI_FIS_TYPE_REG_D2H 0x34
#define AHCI_FIS_TYPE_DMA_ACT 0x39
#define AHCI_FIS_TYPE_DMA_SETUP 0x41
#define AHCI_FIS_TYPE_DATA 0x46
#define AHCI_FIS_TYPE_BIST 0x58
#define AHCI_FIS_TYPE_PIO_SETUP 0x5f
#define AHCI_FIS_TYPE_DEV_BITS 0xa1

typedef enum AhciPortTypeEnum {
  NONE = 0,
  SATA = 1,
  SEMB = 2,
  PM = 3,
  SATAPI = 4,
} AhciPortType;

typedef struct AhciHbaPortStruct {
  u32 command_list_base;
  u32 command_list_base_upper;
  u32 fis_base_address;
  u32 fis_base_address_upper;
  u32 interrupt_status;
  u32 interrupt_enable;
  u32 cmd_sts;
  u32 rsv0;
  u32 task_file_data;
  u32 signature;
  u32 sata_status;
  u32 sata_control;
  u32 sata_error;
  u32 sata_active;
  u32 command_issue;
  u32 sata_notification;
  u32 fis_switch_control;
  u32 rsv1[11];
  u32 vendor[4];
} __attribute__((packed)) AhciHbaPort;

typedef struct AhciHbaMemoryStruct {
  u32 host_capability;
  u32 global_host_control;
  u32 interrupt_status;
  u32 ports_implemented;
  u32 version;
  u32 ccc_control;
  u32 ccc_ports;
  u32 enclosure_management_location;
  u32 enclosure_management_control;
  u32 host_capabilities_extended;
  u32 bios_handoff_ctrl_sts;
  u8 rsv0[0x74];
  u8 vendor[0x60];
  AhciHbaPort ports[1];
} __attribute__((packed)) AhciHbaMemory;

typedef struct AhciHbaCommandHeaderStruct {
  u8 command_fis_length : 5;
  u8 atapi : 1;
  u8 write : 1;
  u8 prefetchable : 1;

  u8 reset : 1;
  u8 bist : 1;
  u8 clear_busy : 1;
  u8 rsv0 : 1;
  u8 port_multiplier : 4;

  u16 prdt_length;
  u32 prdb_count;
  u32 command_table_base_address;
  u32 command_table_base_address_upper;
  u32 rsv1[4];
} __attribute__((packed)) AhciCommandHeader;

typedef struct AhciFisH2DStruct {
  u8 fis_type;
  u8 port_multiplier : 4;
  u8 reserved0 : 3;
  u8 cmd_control : 1;
  u8 cmd;
  u8 feature_low;
  u8 lba_low[3];
  u8 device_register;
  u8 lba_high[3];
  u8 feature_high;
  u8 count[2];
  u8 iso_command_completion;
  u8 control;
  u8 reserved1[4];
} __attribute__((packed)) AhciFisH2D;

typedef struct AhciHbaPrdtEntryStruct {
  u32 data_base[2];
  u32 reserved0;
  u32 byte_count : 22;
  u32 reserved1 : 9;
  u32 interrupt_on_completion : 1;
} __attribute__((packed)) AhciHbaPrdtEntry;

typedef struct AhciHbaCommandTableStruct {
  u8 command_fis[64];
  u8 atapi_cmd[16];
  u8 reserved0[48];
  AhciHbaPrdtEntry prdt_entry[];
} __attribute__((packed)) AhciHbaCommandTable;

typedef struct AhciPortStruct {
  AhciHbaPort *hba_port;
  AhciPortType port_type;
  u8 *buffer;
  u8 port_number;
} AhciPort;

typedef struct {
  PciType0 *ahci_device;
  AhciHbaMemory *abar;
  AhciPort *ports[32];
  u32 port_count;
} Ahci;

Ahci *ahci_init(PciType0 *h);
void ahci_probe(Ahci *driver);
AhciPortType ahci_check_port_type(AhciHbaPort *port);

void ahci_port_configure(AhciPort *p);
void ahci_port_start_cmd(AhciPort *p);
void ahci_port_stop_cmd(AhciPort *p);
bool ahci_port_read(AhciPort *p, u64 sector, u32 count, void *buffer);
bool ahci_port_write(AhciPort *p, u64 sector, u32 count, void *buffer);
bool ahci_port_check(AhciPort *p);
