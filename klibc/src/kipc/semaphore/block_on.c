#include <kipc/semaphore.h>

void block_on(semaphore_t *s) {
  *s += 1;
  while (*s) {
    asm volatile("nop");
  }
}
