#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

#define STR_OK 0
#define STR_NOBUF 1
#define STR_OVERFLOW 2
#define STR_LOWBASE 3

i32 ntos(char *buffer, ssize_t integer, uint8_t base, size_t length,
         bool unsign, bool pad);
i32 reverse(char *buffer, size_t len);
size_t strlen(char *string);
size_t strnext(char *string, char token);
bool strncmp(char *a, char *b, size_t size);
bool strnlcmp(char *a, char *b, size_t size, size_t limit);
