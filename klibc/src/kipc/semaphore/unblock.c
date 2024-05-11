#include "kipc/semaphore.h"

void unblock(semaphore_t *s) { *s -= 1; }
