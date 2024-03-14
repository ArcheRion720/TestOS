#include "print.h"
#include "pit.h"
#include "utils.h"
#include "x86/hal.h"

uint64_t ticks;
void init_pit()
{
    //channel 0 | lobyte/hibyte | square wave generator | binary
    outport8(PIT_CMD, 0b00110110);

    outport8(PIT_CH0_DATA, PIT_DIVISOR_LOW);
    outport8(PIT_CH0_DATA, PIT_DIVISOR_HIGH);

    //register_isr_handler(32, &pit_handler);

    print_fmt("Initialised PIT\n");
}