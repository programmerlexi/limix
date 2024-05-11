#pragma once
#include "types.h"
#include <stddef.h>

void *kmemcpy(void *destination, const void *source, size_t count);
void *kmemset(void *destination, i32 value, size_t count);
void *kmemmove(void *destination, const void *source, size_t count);
i32 kmemcmp(const void *destination, const void *source, size_t count);
