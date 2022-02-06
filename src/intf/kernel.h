#include <stdint.h>
#include <stddef.h>
#include "bootloader.h"
#include "interrupts.h"
#include "pic.h"
#include "pit.h"
#include "ps2.h"
#include "utils.h"
#include "gfx.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "pci.h"
#include "rtc.h"
#include "storage/ahci.h"
#include "storage/drive.h"
#include "tasking.h"

void initialize(struct stivale2_struct* stivale)
{
    init_terminal(stivale);
    init_pic();
    init_intdt();
    init_pit();
    init_keyboard();
    init_memory_manager(stivale);    
    init_virtual_memory_manager(stivale);    
    init_graphics(stivale);
    init_pci();
    init_rtc();
    init_ahci();
    init_tasking();
}