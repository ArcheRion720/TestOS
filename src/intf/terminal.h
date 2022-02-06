#pragma once
#include <stddef.h>
#include "stivale2.h"
#include "bootloader.h"

#define TEXT(x) x, sizeof(x)-1

void init_terminal(struct stivale2_struct* stivale);
void terminal_write(const char* string, size_t length);