#pragma once

#include <stdint.h>
#include <utils/errors.h>

struct tnode;
#include <fs/devfs.h>

typedef struct vnode {
  uint64_t refcount;
  uint64_t type;
  struct device *storage;
  void *data;
  error_t (*open)(struct vnode *node);
  error_t (*read)(struct vnode *node, char *buffer, uint64_t count,
                  uint64_t offset);
  error_t (*write)(struct vnode *node, char *buffer, uint64_t count,
                   uint64_t offset);
  error_t (*close)(struct vnode *node);
  error_t (*ioctl)(struct vnode *node, ioctl_t ctl);
  error_t (*dirent)(struct vnode *node, struct tnode *res, uint64_t index);
  error_t (*dirents)(struct vnode *node, uint64_t *count);
  error_t (*newent)(struct vnode *node, struct tnode *new);
} vnode_t;

typedef struct tnode {
  uint64_t name_length;
  char *name;
  vnode_t *inode;
  uint64_t child_count;
  struct tnode *children;
  struct tnode *next;
} tnode_t;

void vfs_init();
error_t vfs_mount(char *path, device_t *dev, vnode_t node);
