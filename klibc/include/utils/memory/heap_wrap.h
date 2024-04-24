#pragma once

#include <stddef.h>

void *malloc(size_t count);
void free(void *pointer);
void *realloc(void *pointer, size_t old, size_t new);
void *clone(void *pointer, size_t size);
