#pragma once
#include <stdint.h>
#include <stdarg.h>

void itoah(uint64_t value, char* buffer);

__attribute__((format (printf, 1, 2))) int printf(const char* format, ...);