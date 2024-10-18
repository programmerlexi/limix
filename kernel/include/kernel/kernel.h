#pragma once

__attribute__((noreturn)) void hcf();
__attribute__((noreturn)) void kernel_panic_error(const char *error);
__attribute__((noreturn)) void kernel_panic();
