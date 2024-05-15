#pragma once

#include "types.h"

typedef i64 semaphore_t;

void block_on(semaphore_t *semaphore);
void block_on_count(semaphore_t *semaphore, u64 count);
void unblock(semaphore_t *semaphore);
