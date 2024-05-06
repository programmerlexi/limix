#include <fs/devfs.h>
#include <fs/vfs.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/strings/xstr.h>

static vfs_t *filesystems;
static uint64_t fs_count;

void vfs_init() {
  fs_count = 1;

  filesystems = malloc(sizeof(vfs_t));
  filesystems->name = to_xstr("dev");
  devfs_init();
  devfs_bind(filesystems);
  devfs_reload();
}
