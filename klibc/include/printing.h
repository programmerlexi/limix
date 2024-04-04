#pragma once

#include <stdarg.h>

void kprint(char *);
void kprintc(char);

#define print kprint

void printf(char *, ...);
void vprintf(char *, va_list);
