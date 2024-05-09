#pragma once

#include <stddef.h>

void *malloc(size_t count);
void free(void *pointer);
void *realloc(void *pointer, size_t old_size, size_t new_size);
void *clone(void *pointer, size_t size);
