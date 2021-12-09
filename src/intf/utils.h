#pragma once
#include <stdint.h>
#include <stdarg.h>

void itoah(uint64_t value, char* buffer);
void memset(uint8_t* addr, uint8_t value, uint64_t count);
void str_cpy(uint8_t* src, uint8_t* dest, uint8_t count);

__attribute__((format (printf, 1, 2))) int printf(const char* format, ...);