#pragma once
#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t count);
void *memset(void *dst, int val, size_t count);
void *memmove(void *dst, const void *src, size_t count);
int memcmp(const void *a, const void *b, size_t count);
