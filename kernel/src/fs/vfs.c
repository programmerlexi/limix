#include <fs/vfs.h>
#include <kernel.h>

int open(vhandle_t *handle, char *file) {
  kernel_panic_error("Call to unimplemented function 'open'.");
  return -1;
}
int write(vhandle_t *handle, char *buffer, uint64_t s) {
  kernel_panic_error("Call to unimplemented function 'write'.");
  return -1;
}
int read(vhandle_t *handle, char *buffer, uint64_t s) {
  kernel_panic_error("Call to unimplemented function 'read'.");
  return -1;
}
int close(vhandle_t *handle) {
  kernel_panic_error("Call to unimplemented function 'close'.");
  return -1;
}
int flush(vhandle_t *handle) {
  kernel_panic_error("Call to unimplemented function 'flush'.");
  return -1;
}
int chown(vhandle_t *handle, uint8_t type, uint16_t id) {
  kernel_panic_error("Call to unimplemented function 'chown'.");
  return -1;
}
int chmod(vhandle_t *handle, uint8_t new_mod) {
  kernel_panic_error("Call to unimplemented function 'chmod'.");
  return -1;
}
int gmod(vhandle_t *handle, uint16_t *buffer) {
  kernel_panic_error("Call to unimplemented function 'gmod'.");
  return -1;
}
int gown(vhandle_t *handle, uint8_t type, uint16_t *buffer) {
  kernel_panic_error("Call to unimplemented function 'gown'.");
  return -1;
}
