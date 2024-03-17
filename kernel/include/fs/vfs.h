#pragma once

#include <stdbool.h>
#include <stdint.h>

#define VFS_TYPE_FILE 0
#define VFS_TYPE_DIRECTORY 1
#define VFS_TYPE_SYMBOLIC_LINK 2
#define VFS_TYPE_MOUNT_POINT 3

typedef struct {
  uint64_t offset;
  uint64_t size;
  uint8_t *buffer;
} vbuffer_t;

typedef struct {
  uint16_t user;
  uint16_t group;
  uint16_t permissions;
  uint64_t size;
  uint64_t ctime;
  uint64_t mtime;
  uint64_t atime;
  uint64_t type;
} vinfo_t;

typedef struct {
  uint32_t filesystem;
  bool backed;
  uint32_t filename_len;
  char *filename;
  uint64_t open_count;
  uint64_t buffer_count;
  uint8_t **buffers;
  vinfo_t file_info;
  uint64_t filesystem_handle;
} vnode_t;

typedef struct {
  int (*mount)(vnode_t *);
  int (*populate_buffers)(vnode_t *);
  int (*flush_buffers)(vnode_t *);
  int (*populate_info)(vnode_t *);
  int (*flush_info)(vnode_t *);
  int (*handle_open)(vnode_t *);
  int (*handle_close)(vnode_t *);
} vfilesystem_t;

typedef struct {
  vnode_t *node;
  uint64_t cursor;
} vhandle_t;

int open(vhandle_t *, char *);
int write(vhandle_t *, char *, uint64_t);
int read(vhandle_t *, char *, uint64_t);
int close(vhandle_t *);
int flush(vhandle_t *);
int chown(vhandle_t *, uint8_t, uint16_t);
int chmod(vhandle_t *, uint8_t);
int gmod(vhandle_t *, uint16_t *);
int gown(vhandle_t *, uint8_t, uint16_t *);
