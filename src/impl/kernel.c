#include "kernel.h"

void initialize()
{
    init_serial(COM1);
    init_terminal();
    init_intdt();
    init_pic();
    init_pit();
    init_keyboard();
    init_memory_manager();    
    init_gdt();
    init_pci();
    init_rtc();
    init_ahci();
}

void kernel_start(void)
{
    initialize();

    for(;;)
    {
        asm("hlt");
    }
}