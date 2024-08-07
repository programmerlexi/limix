#pragma once

#include "libk/types.h"

#define SMB_TYPE_BIOS 0
#define SMB_TYPE_SYSTEM 1
#define SMB_TYPE_MAINBOARD 2
#define SMB_TYPE_CHASIS 3
#define SMB_TYPE_PROCESSOR 4
#define SMB_TYPE_CACHE 7
#define SMB_TYPE_SYSTEM_SLOTS 9
#define SMB_TYPE_PHYSICAL_MEMORY_ARRAY 16
#define SMB_TYPE_MEMORY_DEVICE 17
#define SMB_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS 19
#define SMB_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS 20
#define SMB_TYPE_SYSTEM_BOOT 32

typedef struct {
  char anchor[5];
  u8 ep_checksum;
  u8 ep_lenght;
  u8 smb_major;
  u8 smb_minor;
  u8 smb_docrev;
  u8 ep_revision;
  u8 reserved;
  u16 st_max_size;
  uptr st_addr;
} SmbiosEntryPoint64;

typedef struct {
  u8 type;
  u8 length;
  u16 handle;
} SmbiosHeader;

void smb_init();
usz smb_struct_len(SmbiosHeader *h);
