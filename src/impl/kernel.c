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

extern uint8_t test_elf_file[];

extern void user_jmp(uintptr_t rsp, uintptr_t rip);

void kernel_start(void)
{
    initialize();

    process_t* proc = process_obj_create();
    proc->priority = 100;
    proc->pcid = 150;

    proc->registers.cs = GDT_SEGMENT_OFFSET(GDT_CS3_64) | 3;
    proc->registers.ds = GDT_SEGMENT_OFFSET(GDT_DS3_64) | 3;
    proc->registers.es = GDT_SEGMENT_OFFSET(GDT_DS3_64) | 3;
    proc->registers.fs = GDT_SEGMENT_OFFSET(GDT_DS3_64) | 3;
    proc->registers.gs = GDT_SEGMENT_OFFSET(GDT_DS3_64) | 3;
    proc->registers.rflags = REG_FLAG_IF | 2;

    uintptr_t elf_entry = load_elf((elf_header_t*)&test_elf_file, proc);

    vmm_load_memmap(proc->registers.cr3);
    user_jmp(proc->registers.rsp, proc->registers.rip);

    // schedule_process(proc);

    // print_fmt("ELF loaded!");
    // scheduler_start();

    

    for(;;)
    {
        asm("hlt");
    }
}