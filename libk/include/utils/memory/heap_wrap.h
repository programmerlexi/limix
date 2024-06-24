#pragma once

#include "libk/types.h"

void *kmalloc(usz count);
void kfree(void *pointer);
void *krealloc(void *pointer, usz old_size, usz new_size);
void *clone(void *pointer, usz size);
