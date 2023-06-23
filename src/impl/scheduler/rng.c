#include "scheduler/rng.h"

uint64_t rng(rng_state_t* rng_state)
{
    rng_state_t state = *rng_state;
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return (*rng_state = state * 0x2545F4914F6CDD1DULL);
}

uint32_t rng_next(rng_state_t* state, uint32_t low, uint32_t high)
{
    uint32_t rand = (uint32_t)(rng(state) >> 32);
    return rand % (high + 1 - low) + low;
}