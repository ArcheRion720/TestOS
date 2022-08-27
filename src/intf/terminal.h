#pragma once
#include <stddef.h>

#define TEXT(x) x, sizeof(x)-1

void init_terminal();
void terminal_write(const char* string, size_t length);