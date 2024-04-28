#include <fs/devfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <kernel.h>
#include <mm/heap.h>
#include <utils/memory/safety.h>

static tnode_t *root;

static ramfs_t *ram;

void vfs_init() {
  root = (tnode_t *)root;
  nullsafe_error(root, "Couldn't allocate VFS root tag");
  ram = (ramfs_t *)malloc(sizeof(ramfs_t));
  nullsafe_error(ram, "Couldn't allocate temporary root fs");
  ramfs_new(ram);
  ramfs_mount(ram, &root->inode);
  devfs_init();
  // devfs_mount(root);
}
