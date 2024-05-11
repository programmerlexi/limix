#pragma once

#include "types.h"
#include "utils/strings/xstr.h"
#include <stdint.h>

typedef struct file {
  xstr_t name;
  u64 size;
  i32 (*read)(u64, u64, char *, struct file *);
  i32 (*write)(u64, u64, char *, struct file *);
  void *data;
} file_t;

typedef struct directory {
  xstr_t name;
  u64 directory_count;
  u64 file_count;
  struct directory **directories;
  file_t **files;
} directory_t;

typedef struct vfs {
  xstr_t name;
  directory_t *root;
  struct vfs *next;
} vfs_t;

void vfs_init();
vfs_t *vfs_fs(char *name);
i32 vfs_find_directory(directory_t **result, char *path);
i32 vfs_find_file(file_t **result, char *path);
i32 vfs_type(char *path, u64 *type);
vfs_t *vfs_make(char *name);
