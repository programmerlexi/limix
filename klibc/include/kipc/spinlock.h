#pragma once

#include <stdint.h>

void spinlock(uint32_t *);
void spinunlock(uint32_t *);
