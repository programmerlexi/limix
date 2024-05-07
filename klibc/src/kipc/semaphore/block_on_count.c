#include <kipc/semaphore.h>

void block_on_count(semaphore_t *s, uint64_t count) {
  *s += count;
  while (*s > 0) {
    asm volatile("nop");
  }
}
