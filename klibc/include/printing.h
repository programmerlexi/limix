#pragma once

#include <stdarg.h>

void kprint(char *);
void kprintc(char);

void kprintf(char *, ...);
void kvprintf(char *, va_list);
