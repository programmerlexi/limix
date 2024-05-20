#pragma once
#include "libk/types.h"

void *kmemcpy(void *destination, const void *source, usz count);
void *kmemset(void *destination, i32 value, usz count);
void *kmemmove(void *destination, const void *source, usz count);
i32 kmemcmp(const void *destination, const void *source, usz count);
