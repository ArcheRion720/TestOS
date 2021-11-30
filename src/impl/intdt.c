#include "intdt.h"
#include "hal.h"

idtr_descriptor_t IDT[256];
extern void interrupt_ISR();

void init_intdt()
{
    for(int i = 0; i < 32; i++)
    {
       register_intdt(i, (uint64_t)interrupt_ISR);
    }

    idtr_t ptr = (idtr_t)
    {
        limit: (uint16_t)(256 * 8 - 1),
        base: (uint64_t)IDT,
    };

    __asm__("lidt %0" :: "m"(ptr));
    __asm__("sti");

    printf("Initialised interrupts\n");
}

void register_intdt(uint32_t code, uint64_t addr)
{
    IDT[code].offset015 = (addr & 0xffff);
    IDT[code].selector = 0x28;
    IDT[code].zero = 0;
    IDT[code].type = 0x8E;
    IDT[code].offset1631 = (addr >> 16) & 0xFFFF;
    IDT[code].offset3263 = (addr >> 32);
    IDT[code].zerohigh = 0;
}

void interrupt_handler()
{
    asm ("hlt");
}