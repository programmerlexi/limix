#pragma once

#define nullsafe_with(x, b)                                                    \
  if (!x)                                                                      \
  b
#define nullsafe(x) nullsafe_with(x, return)
