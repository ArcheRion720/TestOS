#pragma once
#include <stddef.h>
#include "stivale2.h"
#include "bootloader.h"

void init_terminal(struct stivale2_struct* stivale);
void debug_write(const char* string, size_t length);