#include <stdint.h>
#include <stddef.h>

void* memset(void* dst, int sval, size_t count) {
    if(!count){return dst;} // nothing to set?
    uint64_t addr = (uint64_t)dst;
    uint64_t val = (sval & 0xFF); // create a 64-bit version of 'sval'
    val |= ((val << 8) & 0xFF00);
    val |= ((val << 16) & 0xFFFF0000);
    val |= ((val << 32) & 0xFFFFFFFF00000000);
    
    while(count >= 8){ *(uint64_t*)addr = (uint64_t)val; addr += 8; count -= 8; }
    while(count >= 4){ *(uint32_t*)addr = (uint32_t)val; addr += 4; count -= 4; }
    while(count >= 2){ *(uint16_t*)addr = (uint16_t)val; addr += 2; count -= 2; }
    while(count >= 1){ *(uint8_t*)addr = (uint8_t)val; addr += 1; count -= 1; }
    return dst; 
}

int memcmp(const void* s1, const void* s2, size_t n) {
    if(!n){return 0;} // nothing to compare?
    const uint8_t *p1 = (const uint8_t*)s1;
    const uint8_t *p2 = (const uint8_t*)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
    return 0;
}


void* memcpy(void* dst, const void* src, size_t count) {
    if(!count){return dst;} // nothing to copy?
    uint64_t dstaddr = (uint64_t)dst;
    uint64_t srcaddr = (uint64_t)src;
    
    while(count >= 8){ *(uint64_t*)dstaddr = *(uint64_t*)srcaddr; dstaddr += 8; srcaddr += 8; count -= 8; }
    while(count >= 4){ *(uint32_t*)dstaddr = *(uint32_t*)srcaddr; dstaddr += 4; srcaddr += 4; count -= 4; }
    while(count >= 2){ *(uint16_t*)dstaddr = *(uint16_t*)srcaddr; dstaddr += 2; srcaddr += 2; count -= 2; }
    while(count >= 1){ *(uint8_t*)dstaddr = *(uint8_t*)srcaddr; dstaddr += 1; srcaddr += 1; count -= 1; }
    return dst;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
    
    if (src > dest) {
        memcpy(dest, src, n);
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }
    return dest;
}
