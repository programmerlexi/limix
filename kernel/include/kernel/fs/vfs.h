#pragma once

#include <libk/types.h>
#include <libk/utils/strings/xstr.h>

typedef struct FileStruct {
  xstr_t name;
  u64 size;
  i32 (*read)(u64, u64, char *, struct FileStruct *);
  i32 (*write)(u64, u64, char *, struct FileStruct *);
  void *data;
} File;

typedef struct DirectoryStruct {
  xstr_t name;
  u64 directory_count;
  u64 file_count;
  struct DirectoryStruct **directories;
  File **files;
} Directory;

typedef struct VfsStruct {
  xstr_t name;
  Directory *root;
  struct VfsStruct *next;
} Vfs;

void vfs_init();
Vfs *vfs_fs(char *name);
i32 vfs_find_directory(Directory **result, char *path);
i32 vfs_find_file(File **result, char *path);
i32 vfs_type(char *path, u64 *type);
Vfs *vfs_make(char *name);
