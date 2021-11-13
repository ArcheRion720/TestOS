#include <stdint.h>
#include <stddef.h>
#include "bootloader.h"
#include "intdt.h"
#include "pic.h"
#include "ps2.h"
#include "debug.h"
#include "utils.h"
#include "gfx.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "pci.h"

void kernel_start(struct stivale2_struct* stivale)
{
   init_terminal(stivale);
   debug_write(TEXT("Initialized debug temrinal\n"));
   pic_init();
   debug_write(TEXT("Initialized PIC\n"));
   init_intdt();
   debug_write(TEXT("Initialized interrupts\n"));
   init_keyboard();
   debug_write(TEXT("Initialized PS2\n"));
   init_memory_manager(stivale);    
   debug_write(TEXT("Initialized memory... I think\n"));
   init_virtual_memory_manager(stivale);    
   debug_write(TEXT("Virtualized memory... I think\n"));
   init_graphics(stivale);
   debug_write(TEXT("Initialized graphics\n"));
   init_pci();

   pci_entry_t* controller = pci_get_function(0x1, 0x6, 1);
   if(controller == 0x0)
   {
       debug_write(TEXT("No AHCI controller found!\n"));
   }

   //outport32(0xCF8, (uint32_t)((1 << 31) | (0x1F << 11)));
   //debug_number(inport32(0xCFC));

   while(1)
   {
       asm("pause");
   }

   asm("hlt");
}