#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/safety.h>

error_t ramfs_new(ramfs_t *ramfs) {
  ramfs->root = (ramfs_dir_t *)malloc(sizeof(ramfs_dir_t));
  nullsafe_with(ramfs->root, return E_OOM);
  return E_SUCCESS;
}

static error_t _ramfs_dirent(struct vnode *node, struct tnode *res,
                             uint64_t index) {
  return 0;
}
static error_t _ramfs_dirents(struct vnode *node, uint64_t *count) { return 0; }
static error_t _ramfs_newent(struct vnode *node, struct tnode *new) {
  return 0;
}

error_t ramfs_mount(ramfs_t *ramfs, vnode_t **node_ref) {
  *node_ref = (vnode_t *)malloc(sizeof(vnode_t));
  vnode_t *node = *node_ref;
  nullsafe_with(node, return E_OOM);
  node->data = ramfs;
  node->dirent = _ramfs_dirent;
  node->dirents = _ramfs_dirents;
  node->newent = _ramfs_newent;
  return 0;
}
