#include "gdt.h"
#include "memory/pmm.h"
#include "utils.h"
#include "hal.h"
#include "terminal.h"

gdtr_descriptor_t* gdt;

void init_gdt()
{
    gdt = (gdtr_descriptor_t*) malloc(sizeof(gdtr_descriptor_t*) * 9);

    __asm__ ("cli");
    register_gdt(0, 0, 0, 0, 0); //null entry

    register_gdt(
        1, 0, 0xffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN);  //16 bit kernel code

    register_gdt(
        2, 0, 0xffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN);  //16 bit kernel data

    register_gdt(
        3, 0, 0xffffffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_32SEG); //32 bit kernel code

    register_gdt(
        4, 0, 0xffffffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_32SEG); //32 bit kernel data

    register_gdt(
        5, 0, 0,
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_LONG); //64 bit kernel code
        
    register_gdt(
        6, 0, 0, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_LONG); //64 bit kernel data

    register_gdt(
        7, 0, 0,
        GDT_KERNEL_BASE | GDT_DPL_USER | GDT_EXEC,
        GDT_FL_GRAN | GDT_FL_LONG); //64 bit client code

    register_gdt(
        8, 0, 0, 
        GDT_KERNEL_BASE | GDT_DPL_USER,
        GDT_FL_GRAN | GDT_FL_LONG); //64 bit client data

    gdtr_t ptr = {
        .limit = sizeof(gdtr_descriptor_t) * 9 - 1,
        .base = (uint64_t)gdt
    };

    out_serial_str(COM1, TEXT("pre"));

    __asm__ ("lgdt %0" :: "m"(ptr));
    __asm__ ("sti");

    out_serial_str(COM1, TEXT("post"));
}

void register_gdt(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[index].limit015 = (uint16_t)limit;
    gdt[index].base015  = (uint16_t)base;
    gdt[index].base1623 = (uint8_t)(base >> 16);
    gdt[index].access   = access;
    gdt[index].flags    = flags | ((limit >> 16) & 0xF);
    gdt[index].base2431 = (uint8_t) (base >> 24);
}