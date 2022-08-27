#include <stdint.h>
#include <stddef.h>
#include "hal.h"
#include "memory/pmm.h"
#include "pci.h"
#include "rtc.h"
#include "storage/ahci.h"
#include "storage/drive.h"

void initialize()
{
    init_serial(COM1);
    init_terminal();
    init_intdt();
    init_pic();
    init_pit();
    init_keyboard();
    init_memory_manager();    
    init_pci();
    init_rtc();
    init_ahci();
}