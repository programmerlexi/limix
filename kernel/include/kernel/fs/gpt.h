#pragma once

#include <kernel/fs/mbr.h>
#include <kernel/hw/devman/devman.h>
#include <stdbool.h>

typedef struct {
  u8 signature[8];
  u32 gpt_revision;
  u32 header_size;
  u32 crc_checksum;
  u32 reserved;
  u64 lba_of_header;
  u64 lba_of_alternate_header;
  u64 first_usable_block;
  u64 last_usable_block;
  u8 guid[16];
  u64 gp_array_lba;
  u32 partition_entries;
  u32 partition_entry_size;
  u32 crc_partition_array;
} __attribute__((packed)) GptPartitionHeader;

typedef struct {
  u8 pt_guid[16];
  u8 up_guid[16];
  u64 start;
  u64 end;
  u64 attributes;
  u8 partition_name[];
} __attribute__((packed)) GptPartition;

typedef struct {
  MbrBootsector pmbr __attribute__((aligned(0x200)));
  GptPartitionHeader partition_header __attribute__((aligned(0x200)));
} __attribute__((packed)) GptHeader;

void gpt_init(DevmanStorageAccessHandle ah);
