#include <kipc.h>

void block_on(semaphore_t* s) {
    *s+=1;
    while (*s) {
        asm volatile("nop");
    }
}

void block_on_count(semaphore_t* s, uint64_t count) {
    *s += count;
    while (*s > 0) {
        asm volatile("nop");
    }
}

void unblock(semaphore_t* s) {
    *s-=1;
}
