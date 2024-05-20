#include "libk/ipc/semaphore.h"

void unblock(semaphore_t *s) { *s -= 1; }
