#include <libk/types.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/strings/strings.h>
#include <stddef.h>

i32 ntos(char *buf, isz i, u8 base, usz length, bool unsign, bool pad) {
  if (!length || buf == NULL)
    return STR_NOBUF;

  if (base < 2)
    return STR_LOWBASE;

  kmemset(buf, 0, length);
  if (pad)
    kmemset(buf, '0', length);
  buf[length] = 0;

  if (i == 0) {
    buf[0] = '0';
    return STR_OK;
  }

  usz p = 0;
  if (!unsign) {
    isz n = i;
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
    usz n = i;
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
