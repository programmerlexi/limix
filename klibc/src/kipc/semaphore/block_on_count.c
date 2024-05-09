#include <kipc/semaphore.h>
#include <types.h>

void block_on_count(semaphore_t *s, u64 count) {
  *s += count;
  while (*s > 0) {
    asm volatile("nop");
  }
}
