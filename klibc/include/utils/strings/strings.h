#pragma once
#include "types.h"
#include <stdbool.h>

#define STR_OK 0
#define STR_NOBUF 1
#define STR_OVERFLOW 2
#define STR_LOWBASE 3

i32 ntos(char *buffer, isz integer, u8 base, usz length, bool unsign, bool pad);
i32 reverse(char *buffer, usz len);
usz kstrlen(char *string);
usz kstrnext(char *string, char token);
bool kstrncmp(char *a, char *b, usz size);
bool kstrnlcmp(char *a, char *b, usz size, usz limit);
