#include <math/lib.h>
#include <stdbool.h>
#include <stdint.h>
#include <types.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

i32 ntos(char *buf, ssize_t i, u8 base, size_t length, bool unsign, bool pad) {
  if (!length || buf == NULL)
    return STR_NOBUF;

  if (base < 2)
    return STR_LOWBASE;

  memset(buf, 0, length);
  if (pad)
    memset(buf, '0', length);

  if (i == 0) {
    buf[0] = '0';
    return STR_OK;
  }

  size_t p = 0;
  if (!unsign) {
    ssize_t n = i;
    while (n > 0 && p < length) {
      char d = (n % base);
      if (d < 10)
        buf[p] = '0' + d;
      else if (d < 35)
        buf[p] = 'a' + d - 10;
      else if (d < 60)
        buf[p] = 'A' + d - 35;
      else
        buf[p] = '?';
      n /= base;
      p++;
    }
    if (i < 0)
      buf[p] = '-';
    else
      buf[p] = '+';
  } else {
    size_t n = i;
    while (n > 0 && p < length) {
      char d = (n % base);
      if (d < 10)
        buf[p] = '0' + d;
      else if (d < 35)
        buf[p] = 'a' + d - 10;
      else if (d < 60)
        buf[p] = 'A' + d - 35;
      else
        buf[p] = '?';
      n /= base;
      p++;
    }
  }
  if (!unsign)
    p++;
  if (!pad)
    reverse(buf, p);
  else
    reverse(buf, length);
  if (!(p < length))
    return STR_OVERFLOW;
  else
    return STR_OK;
}
