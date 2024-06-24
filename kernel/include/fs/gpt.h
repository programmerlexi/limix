#pragma once

#include "kernel/fs/mbr.h"
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
} __attribute__((packed)) gpt_partition_header_t;

typedef struct {
  mbr_t pmbr __attribute__((aligned(0x200)));
  gpt_partition_header_t partition_header __attribute__((aligned(0x200)));
} __attribute__((packed)) gpt_header_t;

void gpt_init(void *drv,
              bool (*read)(void *drv, u64 lba, u32 count, void *buffer));
