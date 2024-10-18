#pragma once

#include <libk/types.h>

typedef struct {
  u8 attribute;
  u8 start_chs[3];
  u8 partition_type;
  u8 end_chs[3];
  u32 start_lba;
  u32 sectors;
} __attribute__((packed)) MbrEntry;

typedef struct {
  u8 bootstrap[440];
  u32 udid;
  u16 reserved;
  MbrEntry partitions[4];
  u8 bootsector_signature[2];
} __attribute__((packed)) MbrBootsector;
