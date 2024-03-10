#pragma once
#include <stdbool.h>

#define STR_OK 0
#define STR_NOBUF 1
#define STR_OVERFLOW 2
#define STR_LOWBASE 3

int ntos(char *buf, int i, int base, int length, bool unsign, bool pad);
int reverse(char *buf, int len);
