#include <libk/utils/memory/memory.h>
#include <string.h>

void *memcpy(void *a, const void *b, size_t c) { return kmemcpy(a, b, c); }
void *memset(void *a, int b, size_t c) { return kmemset(a, b, c); }
int memcmp(const void *a, const void *b, size_t c) { return kmemcmp(a, b, c); }
void __stack_chk_fail() {}
