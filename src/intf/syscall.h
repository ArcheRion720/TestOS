#pragma once
#include <stdint.h>

void init_syscall();
void syscall_handler(uint64_t sp3, uint64_t ip3);