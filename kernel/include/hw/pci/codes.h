#pragma once

// Class codes
#define PCI_CLASS_UNCLASSIFIED 0
#define PCI_CLASS_MASS_STORAGE 1
#define PCI_CLASS_NETWORK 2
#define PCI_CLASS_DISPLAY 3
#define PCI_CLASS_MULTIMEDIA 4
#define PCI_CLASS_MEMORY 5
#define PCI_CLASS_BRIDGE 6
#define PCI_CLASS_COM 7
#define PCI_CLASS_BASE 8
#define PCI_CLASS_INPUT 9
#define PCI_CLASS_DOCK 10
#define PCI_CLASS_PROCESSOR 11
#define PCI_CLASS_BUS 12
#define PCI_CLASS_WIRELESS 13
#define PCI_CLASS_INTELLIGENT 14
#define PCI_CLASS_SATELLITE 15
#define PCI_CLASS_ENCRYPTION 16
#define PCI_CLASS_SIGNALPROC 17
#define PCI_CLASS_ACCEL 18
#define PCI_CLASS_NONESSENT 19
#define PCI_CLASS_COPROC 64
#define PCI_CLASS_VENDORSPEC 255

// Subclass codes: Unclassified
#define PCI_SUBCLASS_UNCLASSIFIED_NON_VGA_COMPAT 0
#define PCI_SUBCLASS_UNCLASSIFIED_VGA_COMPAT 1

// Subclass codes: Mass Storage
#define PCI_SUBCLASS_MASS_STORAGE_SCSI 0
#define PCI_SUBCLASS_MASS_STORAGE_IDE 1
#define PCI_SUBCLASS_MASS_STORAGE_FLOPPY 2
#define PCI_SUBCLASS_MASS_STORAGE_IPI 3
#define PCI_SUBCLASS_MASS_STORAGE_RAID 4
#define PCI_SUBCLASS_MASS_STORAGE_ATA 5
#define PCI_SUBCLASS_MASS_STORAGE_SATA 6
#define PCI_SUBCLASS_MASS_STORAGE_SA_SCSI 7
#define PCI_SUBCLASS_MASS_STORAGE_NVM 8
#define PCI_SUBCLASS_MASS_STORAGE_OTHER 0x80

// Subclass codes: Network
#define PCI_SUBCLASS_NETWORK_ETHERNET 0
#define PCI_SUBCLASS_NETWORK_TOKEN_RING 1
#define PCI_SUBCLASS_NETWORK_FDDI 2
#define PCI_SUBCLASS_NETWORK_ATM 3
#define PCI_SUBCLASS_NETWORK_ISDN 4
#define PCI_SUBCLASS_NETWORK_WORLDFIP 5
#define PCI_SUBCLASS_NETWORK_PICMG 6
#define PCI_SUBCLASS_NETWORK_INFINIBAND 7
#define PCI_SUBCLASS_NETWORK_FABRIC 8
#define PCI_SUBCLASS_NETWORK_OTHER 0x80

// Subclass codes: Display
#define PCI_SUBCLASS_DISPLAY_VGA 0
#define PCI_SUBCLASS_DISPLAY_XGA 1
#define PCI_SUBCLASS_DISPLAY_3D 2
#define PCI_SUBCLASS_DISPLAY_OTHER 0x80

// Subclass codes: Multimedia
#define PCI_SUBCLASS_MULTIMEDIA_VIDEO 0
#define PCI_SUBCLASS_MULTIMEDIA_AUDIO 1
#define PCI_SUBCLASS_MULTIMEDIA_TELEPHONY 2
#define PCI_SUBCLASS_MULTIMEDIA_AUDIODEV 3
#define PCI_SUBCLASS_MULTIMEDIA_OTHER 0x80

// Subclass codes: Memory
#define PCI_SUBCLASS_MEMORY_RAM 0
#define PCI_SUBCLASS_MEMORY_FLASH 1
#define PCI_SUBCLASS_MEMORY_OTHER 0x80

// Subclass codes: Bridge
#define PCI_SUBCLASS_BRIDGE_HOST 0
#define PCI_SUBCLASS_BRIDGE_ISA 1
#define PCI_SUBCLASS_BRIDGE_EISA 2
#define PCI_SUBCLASS_BRIDGE_MCA 3
#define PCI_SUBCLASS_BRIDGE_PCI_TO_PCI 4
#define PCI_SUBCLASS_BRIDGE_PCMCIA 5
#define PCI_SUBCLASS_BRIDGE_NUBUS 6
#define PCI_SUBCLASS_BRIDGE_CARDBUS 7
#define PCI_SUBCLASS_BRIDGE_RACEWAY 8
#define PCI_SUBCLASS_BRIDGE_ANOTHER_PCI_TO_PCI 9
#define PCI_SUBCLASS_BRIDGE_INFINIBAND_TO_PCI_HOST 10
#define PCI_SUBCLASS_BRIDGE_OTHER 0x80

// Subclass codes: Com
#define PCI_SUBCLASS_COM_SERIAL 0
#define PCI_SUBCLASS_COM_PARALLEL 1
#define PCI_SUBCLASS_COM_MULTI_SERIAL 2
#define PCI_SUBCLASS_COM_MODEM 3
#define PCI_SUBCLASS_COM_IEEE_488_1_2 4
#define PCI_SUBCLASS_COM_SMART_CARD 5
#define PCI_SUBCLASS_COM_OTHER 0x80

// Subclass codes: Base
#define PCI_SUBCLASS_BASE_PIC 0
#define PCI_SUBCLASS_BASE_DMA 1
#define PCI_SUBCLASS_BASE_TIMER 2
#define PCI_SUBCLASS_BASE_RTC 3
#define PCI_SUBCLASS_BASE_PCI_HOT_PLUG 4
#define PCI_SUBCLASS_BASE_SD 5
#define PCI_SUBCLASS_BASE_IOMMU 6
#define PCI_SUBCLASS_BASE_OTHER 0x80

// Subclass codes: Input
#define PCI_SUBCLASS_INPUT_KEYBOARD 0
#define PCI_SUBCLASS_INPUT_PEN 1
#define PCI_SUBCLASS_INPUT_MOUSE 2
#define PCI_SUBCLASS_INPUT_SCANNER 3
#define PCI_SUBCLASS_INPUT_GAMEPORT 4
#define PCI_SUBCLASS_INPUT_OUTHER 0x80

// Subclass codes: Dock
#define PCI_SUBCLASS_DOCK_GERNERIC 0
#define PCI_SUBCLASS_DOCK_OTHER 0x80

// Subclass codes: Processor
#define PCI_SUBCLASS_PROCESSOR_386 0
#define PCI_SUBCLASS_PROCESSOR_486 1
#define PCI_SUBCLASS_PROCESSOR_PENTIUM 2
#define PCI_SUBCLASS_PROCESSOR_PENTIUM_PRO 3
#define PCI_SUBCLASS_PROCESSOR_ALPHA 16
#define PCI_SUBCLASS_PROCESSOR_POWERPC 32
#define PCI_SUBCLASS_PROCESSOR_MIPS 48
#define PCI_SUBCLASS_PROCESSOR_CO_PROCESSOR 64
#define PCI_SUBCLASS_PROCESSOR_OTHER 0x80

// Subclass codes: Bus
#define PCI_SUBCLASS_BUS_FIREWIRE 0
#define PCI_SUBCLASS_BUS_ACCESS 1
#define PCI_SUBCLASS_BUS_SSA 2
#define PCI_SUBCLASS_BUS_USB 3
#define PCI_SUBCLASS_BUS_FIBRE 4
#define PCI_SUBCLASS_BUS_SMBUS 5
#define PCI_SUBCLASS_BUS_INFINIBAND 6
#define PCI_SUBCLASS_BUS_IPMI 7
#define PCI_SUBCLASS_BUS_SERCOS 8
#define PCI_SUBCLASS_BUS_CANBUS 9
#define PCI_SUBCLASS_BUS_OTHER 0x80

// Subclass codes: Wireless
#define PCI_SUBCLASS_WIRELESS_IRDA 0
#define PCI_SUBCLASS_WIRELESS_IR 1
#define PCI_SUBCLASS_WIRELESS_RF 16
#define PCI_SUBCLASS_WIRELESS_BLUETOOTH 17
#define PCI_SUBCLASS_WIRELESS_BROADBAND 18
#define PCI_SUBCLASS_WIRELESS_802_1A 32
#define PCI_SUBCLASS_WIRELESS_802_1B 33
#define PCI_SUBCLASS_WIRELESS_OTHER 0x80

// Subclass codes: Intelligent
#define PCI_SUBCLASS_ITELLIGENT_I20 0

// Subclass codes: Satellite
#define PCI_SUBCLASS_SATELLITE_TV 1
#define PCI_SUBCLASS_SATELLITE_AUDIO 2
#define PCI_SUBCLASS_SATELLITE_VOICE 3
#define PCI_SUBCLASS_SATELLITE_DATA 4

// Subclass codes: Encryption
#define PCI_SUBCLASS_ENCRYPTION_NETWORK_AND_COMPUTING 0
#define PCI_SUBCLASS_ENCRYPTION_ENTERTAINMENT 16
#define PCI_SUBCLASS_ENCRYPTION_OTHER 0x80

// Subclass codes: Signal Processing
#define PCI_SUBCLASS_SIGPROC_DPIO 0
#define PCI_SUBCLASS_SIGPROC_PERF_COUNTER 1
#define PCI_SUBCLASS_SIGPROC_COM_SYNC 16
#define PCI_SUBCLASS_SIGPROC_SIG_PROC_MANAGE 32
#define PCI_SUBCLASS_SIGPROC_OTHER 0x80

#define PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_SPECIFIC 0
#define PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_AHCI 1
#define PCI_PROGIF_MASS_STORAGE_SATA_VENDOR_SSB 2
