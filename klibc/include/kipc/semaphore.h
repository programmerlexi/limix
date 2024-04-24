#pragma once

#include <stdint.h>

typedef int64_t semaphore_t;

void block_on(semaphore_t *semaphore);
void block_on_count(semaphore_t *semaphore, uint64_t count);
void unblock(semaphore_t *semaphore);
