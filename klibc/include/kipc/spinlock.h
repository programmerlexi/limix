#pragma once

#include <stdint.h>

void spinlock(uint32_t *spinlock);
void spinunlock(uint32_t *spinlock);
