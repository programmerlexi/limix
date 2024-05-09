#pragma once
#include <stddef.h>
#include <types.h>

void *memcpy(void *destination, const void *source, size_t count);
void *memset(void *destination, i32 value, size_t count);
void *memmove(void *destination, const void *source, size_t count);
i32 memcmp(const void *destination, const void *source, size_t count);
