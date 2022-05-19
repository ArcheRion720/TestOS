#include "gdt.h"
#include "memory/pmm.h"

#include "utils.h"

gdtr_descriptor_t* gdt;
tss_t* tss;

static uint8_t tss_stack[8192];

void init_gdt()
{
    gdt = (gdtr_descriptor_t*) malloc(sizeof(gdtr_descriptor_t*) * 11);
    tss = (tss_t*) malloc(sizeof(tss_t));

    register_gdt(0, 0, 0, 0, 0); //null

    register_gdt(
        1, 0, 0xffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN);

    register_gdt(
        2, 0, 0xffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN);

    register_gdt(
        3, 0, 0xffffffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_32SEG);

    register_gdt(
        4, 0, 0xffffffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_32SEG);

    register_gdt(
        5, 0, 0,
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_LONG);
        
    register_gdt(
        6, 0, 0, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_LONG);

    register_gdt(
        7, 0, 0,
        GDT_KERNEL_BASE | GDT_DPL_USER | GDT_EXEC,
        GDT_FL_GRAN | GDT_FL_LONG);

    register_gdt(
        8, 0, 0, 
        GDT_KERNEL_BASE | GDT_DPL_USER,
        GDT_FL_GRAN | GDT_FL_LONG);

    register_tss(9, (uintptr_t)tss, 0b10001001, 0b01000000);

    gdtr_t ptr = (gdtr_t)
    {
        limit: sizeof(gdtr_descriptor_t) * 11 - 1,
        base: (uint64_t)gdt,
    };

    set_tss((uintptr_t)tss_stack + sizeof(tss_stack));

    uint16_t ltr = 0x48;

    __asm__ ("lgdt %0" :: "m"(ptr));
    __asm__ ("ltr %0" :: "r"(ltr));
}

void register_gdt(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[index].limit015 = (uint16_t)limit;
    gdt[index].base015 = (uint16_t)base;
    gdt[index].base1623 = (uint8_t)(base >> 16);
    gdt[index].access = access;
    gdt[index].flags = flags | ((limit >> 16) & 0xF);
    gdt[index].base2431 = (uint8_t) (base >> 24);
}

void register_tss(uint8_t index, uintptr_t tss, uint8_t access, uint8_t flags)
{
    register_gdt(index,     (uint32_t)(tss), 0x68, access, flags);
    register_gdt(index + 1, (uint16_t)(tss >> 48), (uint16_t)(tss >> 32), 0, 0);
}

void set_tss(uintptr_t stack)
{
    tss->rsp0 = stack;
}