#pragma once
#include <stddef.h>
#include "stivale2.h"
#include "bootloader.h"
#include "utils.h"

#define TEXT(x) x, sizeof(x)-1

void init_terminal(struct stivale2_struct* stivale);
void debug_write(const char* string, size_t length);
void debug_number(uint64_t num);
void debug_newl();