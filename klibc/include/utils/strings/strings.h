#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

#define STR_OK 0
#define STR_NOBUF 1
#define STR_OVERFLOW 2
#define STR_LOWBASE 3

int ntos(char *buf, ssize_t i, uint8_t base, size_t length, bool unsign,
         bool pad);
int reverse(char *buf, size_t len);
size_t strlen(char *s);
size_t strnext(char *s, char c);
bool strncmp(char *a, char *b, size_t n);
bool strnlcmp(char *a, char *b, size_t n, size_t l);
