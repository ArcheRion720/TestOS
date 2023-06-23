#pragma once
#include <stdint.h>

typedef uint64_t rng_state_t;

uint32_t rng_next(rng_state_t* state, uint32_t low, uint32_t high);