#include <libk/ipc/semaphore.h>
#include <libk/types.h>

void block_on_count(semaphore_t *s, u64 count) {
  *s += count;
  while (*s > 0) {
    asm volatile("nop");
  }
}
