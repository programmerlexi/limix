#pragma once

#define CONSTRUCTOR(name) __attribute__((constructor)) static void name()
