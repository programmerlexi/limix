#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <fmt.h>
#include <memory.h>

extern void printc(char);

int vprintf(const char *format, va_list list) {
    for (int i = 0; format[i]; i++) {
        char c = format[i];
    }
}

__attribute__((format (printf, 1, 2))) int printf(char* str, ...) {
    va_list list;
    va_start(list, str);
    int i = vprintf(str, list);
    va_end(list);
    return i;
}

