#include "kernel.h"

void kernel_start(struct stivale2_struct* stivale)
{
    initialize(stivale);

    pci_entry_t* controller = pci_get_function(0x1, 0x6, 1);
    if(controller == 0x0)
    {
        debug_write(TEXT("No AHCI controller found!\n"));
    }

    while(1)
    {
        asm("pause");
    }

    asm("hlt");
}