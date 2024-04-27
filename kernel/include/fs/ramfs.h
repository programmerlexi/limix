#pragma once

#include "fs/vfs.h"
#include <stdint.h>
#include <utils/errors.h>

typedef struct ramfs_file {
  uint64_t name_lenght;
  char *name;
  uint8_t *content;
  uint64_t lenght;
  struct ramfs_file *next;
} ramfs_file_t;

typedef struct ramfs_dir {
  uint64_t subdir_count, subfile_count;
  struct ramfs_dir *subdirs;
  ramfs_file_t *subfiles;
  struct ramfs_dir *next;
} ramfs_dir_t;

typedef struct {
  ramfs_dir_t *root;
} ramfs_t;

error_t ramfs_new(ramfs_t *ramfs);
error_t ramfs_mount(ramfs_t *ramfs, vnode_t **node);
