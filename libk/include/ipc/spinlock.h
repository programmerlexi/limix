#pragma once

#include "libk/types.h"

void spinlock(u32 *spinlock);
void spinunlock(u32 *spinlock);
