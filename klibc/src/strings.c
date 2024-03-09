#include <strings.h>
#include <stdbool.h>

void ntos(char *buf, int i, int base, int length, bool unsign) {
    if (!length) return;
    if (i==0) { buf[0] = '0'; buf[1] = 0; return; }
    
}
