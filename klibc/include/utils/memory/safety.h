#pragma once

#define nullsafe_with(x, b)                                                    \
  if (!x)                                                                      \
  b
#define nullsafe(x) nullsafe_with(x, return)
#define nullsafe_error(x, e) nullsafe_with(x, { kernel_panic_error(e); })
