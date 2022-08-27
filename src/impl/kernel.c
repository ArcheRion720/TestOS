#include "kernel.h"

void kernel_start(void)
{
    initialize();
    out_serial_str(COM1, "Hello\n", 6);
    out_serial_str(COM1, "World!\n", 7);

    for(;;)
    {
        asm("hlt");
    }
}

