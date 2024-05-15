#pragma once

#include "types.h"

void *kmalloc(usz count);
void kfree(void *pointer);
void *realloc(void *pointer, usz old_size, usz new_size);
void *clone(void *pointer, usz size);
