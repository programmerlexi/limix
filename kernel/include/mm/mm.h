#pragma once

#include "libk/types.h"
#include <stdbool.h>

uptr mm_fix(uptr ptr);
bool mm_is_physical(uptr ptr);
bool mm_is_virtual(uptr ptr);
