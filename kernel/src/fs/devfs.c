#include <fs/devfs.h>
#include <fs/vfs.h>
#include <mm/heap.h>
#include <stdint.h>
#include <utils/errors.h>
#include <utils/memory/memory.h>

device_t *devs;

error_t _devfs_void_read(char *buffer, uint64_t count, uint64_t offset) {
  memset(buffer, 0, count);
  return 0;
}

error_t _devfs_void_write(char *buffer, uint64_t count, uint64_t offset) {
  return 0;
}

error_t _devfs_void_ctl(ioctl_t ctl) { return 0; }

void devfs_init() {
  devs = (device_t *)malloc(sizeof(device_t));
  devs->name = "void";
  devs->next = (void *)0;
  devs->read = _devfs_void_read;
  devs->write = _devfs_void_write;
  devs->ioctl = _devfs_void_ctl;
}
void devfs_mount(tnode_t *node) {}
