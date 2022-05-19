#include "rng.h"

uint64_t rng(rng_state_t* state)
{
    *state ^= *state >> 12;
    *state ^= *state << 25;
    *state ^= *state >> 27;
    
    return *state * 2685821657736338717ul;
}

uint32_t rng_next(rng_state_t* state, uint32_t low, uint32_t high)
{
    uint32_t rand = (uint32_t)(rng(state) >> 32);

    return (rand / (0xFFFFFFFF / (high - low) + 1)) + low;
}