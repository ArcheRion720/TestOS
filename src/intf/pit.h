#pragma once
#include <stdint.h>

#define PIT_CH0_DATA 0x40
#define PIT_CMD 0x43

//Divisor for 30ms between interrupts
#define PIT_DIVISOR (715909 / 20)
#define PIT_DIVISOR_LOW (PIT_DIVISOR & 0xFF)
#define PIT_DIVISOR_HIGH ((PIT_DIVISOR >> 8) & 0xFF)

void init_pit();
uint64_t pit_read_tick();