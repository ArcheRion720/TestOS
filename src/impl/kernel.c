#include "kernel.h"
#include "print.h"
#include "limine.h"

void debug_char_output(uint8_t c) { out_serial(COM1, c); }

void initialize()
{
    init_gdt();
    init_idt();

    read_cpu_features();

    init_serial(COM1);

    init_print(debug_char_output);

    init_pic();
    init_pit();
    init_keyboard();
    init_memory_manager(); 
    init_virtual_memory();

    init_syscall();
    init_scheduler();
    // init_pci();
    // init_rtc();
    // init_ahci();
}

void kernel_start(void)
{
    initialize();
    scheduler_start();

    for(;;)
    {
        asm("hlt");
    }
}