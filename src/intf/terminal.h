#pragma once
#include <stddef.h>
#include <stdint.h>

#define TEXT(x) x, sizeof(x)-1

void init_terminal();
void terminal_write(const uint8_t* string, size_t length);