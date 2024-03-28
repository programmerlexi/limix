#include <math/lib.h>
#include <stdbool.h>
#include <stdint.h>
#include <types.h>
#include <utils/memory/memory.h>
#include <utils/strings/strings.h>

int ntos(char *buf, ssize_t i, uint8_t base, size_t length, bool unsign,
         bool pad) {
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

int reverse(char *s, size_t len) {
  if (!len || !s)
    return STR_NOBUF;
  char c;
  size_t i, j;
  for (i = 0, j = len - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
  return STR_OK;
}

size_t strlen(char *s) {
  size_t i = 0;
  while (*s) {
    i++;
    s++;
  }
  return i;
}

size_t strnext(char *s, char c) {
  size_t i = 0;
  while (*s && *s != c) {
    i++;
    s++;
  }
  return i;
}

bool strncmp(char *a, char *b, size_t n) {
  if (min(strlen(a), n) != min(strlen(b), n))
    return false;
  int l = min(min(strlen(a), strlen(b)), n);
  for (int i = 0; i < l; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

bool strnlcmp(char *a, char *b, size_t n, size_t l) {
  if (min(strlen(a), l) != min(strlen(b), l))
    return false;
  int sl = min(min(strlen(a), strlen(b)), n);
  for (int i = 0; i < sl; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
