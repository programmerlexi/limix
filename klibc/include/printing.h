#pragma once

#include <stdarg.h>

void kprint(char *str);
void kprintc(char c);

void kprintf(char *fmt, ...);
void kvprintf(char *fmt, va_list values);
