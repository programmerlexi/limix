#pragma once

#include <stdint.h>
#include <utils/strings/xstr.h>

typedef struct file {
  xstr_t name;
  uint64_t size;
  int (*read)(uint64_t, uint64_t, char *, struct file *);
  int (*write)(uint64_t, uint64_t, char *, struct file *);
  void *data;
} file_t;

typedef struct directory {
  xstr_t name;
  uint64_t directory_count;
  uint64_t file_count;
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
int vfs_find_directory(directory_t **result, char *path);
int vfs_find_file(file_t **result, char *path);
int vfs_type(char *path, uint64_t *type);
vfs_t *vfs_make(char *name);
