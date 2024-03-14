#include "x86/gdt.h"

extern void reload_gdt_segments();

static gdtr_descriptor_t gdt[11];
tss_t tss = {0};

void init_gdt()
{
    __asm__ volatile ("cli");
    register_gdt(GDT_NULL, 0, 0, 0, 0);

    register_gdt(
        GDT_CS0_16, 0, 0xffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN);

    register_gdt(
        GDT_DS0_16, 0, 0xffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN);  

    register_gdt(
        GDT_CS0_32, 0, 0xffffffff, 
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_32SEG);

    register_gdt(
        GDT_DS0_32, 0, 0xffffffff, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_32SEG);

    register_gdt(
        GDT_CS0_64, 0, 0,
        GDT_KERNEL_BASE | GDT_EXEC, 
        GDT_FL_GRAN | GDT_FL_LONG);
        
    register_gdt(
        GDT_DS0_64, 0, 0, 
        GDT_KERNEL_BASE, 
        GDT_FL_GRAN | GDT_FL_LONG);

    register_gdt(
        GDT_CS3_64, 0, 0,
        GDT_KERNEL_BASE | GDT_DPL_USER | GDT_EXEC,
        GDT_FL_GRAN | GDT_FL_LONG);

    register_gdt(
        GDT_DS3_64, 0, 0, 
        GDT_KERNEL_BASE | GDT_DPL_USER,
        GDT_FL_GRAN | GDT_FL_LONG);

    gdtr_t ptr = {
        .limit = sizeof(gdtr_descriptor_t) * 11 - 1,
        .base = (uint64_t)gdt
    };

    tss_descriptor_t* tss_desc = (tss_descriptor_t*)&gdt[GDT_TSS];
    uintptr_t tss_addr = (uintptr_t)&tss;
    register_gdt(GDT_TSS, (uint32_t)tss_addr, sizeof(tss), 0x89, 0x40);
    tss_desc->base3263 = (uint32_t)(tss_addr >> 32);

    __asm__ volatile ("lgdt %0" :: "m"(ptr));
    reload_gdt_segments();
    __asm__ volatile ("ltr %w0" :: "r"(GDT_SEGMENT_OFFSET(GDT_TSS)));
    __asm__ volatile ("sti");
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