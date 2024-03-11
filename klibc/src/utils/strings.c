#include <stdbool.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

int ntos(char *buf, int i, int base, int length, bool unsign, bool pad) {
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

  int n, p;
  n = i;
  p = 0;
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
  if (!pad)
    reverse(buf, p);
  else
    reverse(buf, length);
  if (!(p < length))
    return STR_OVERFLOW;
  else
    return STR_OK;
}

int reverse(char *s, int len) {
  if (!len || !s)
    return STR_NOBUF;
  int c, i, j;
  for (i = 0, j = len - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
  return STR_OK;
}
