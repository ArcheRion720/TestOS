#include "pit.h"
#include "utils.h"
#include "tasking.h"

extern uintptr_t isr_stack_top;

uint64_t ticks;
void init_pit()
{
    //channel 0 | lobyte/hibyte | square wave generator | binary
    outport8(PIT_CMD, 0b00110110);

    outport8(PIT_CH0_DATA, PIT_DIVISOR_LOW);
    outport8(PIT_CH0_DATA, PIT_DIVISOR_HIGH);

    //register_isr_handler(32, &pit_handler);

    log("Initialised PIT");
}