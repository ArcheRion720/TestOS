#include "elf.h"
#include "print.h"
#include "utils.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "scheduler/scheduler.h"

#include <stdint.h>

void print_elf_header(elf_header_t* elf)
{
    print_fmt("\nReading ELF file:\n");
    print_fmt("Magic:\t{xbyte} {xbyte} {xbyte} {xbyte}\n", &elf->magic[0], &elf->magic[1], &elf->magic[2], &elf->magic[3]);
    print_fmt("Bits:\t\t");
    switch(elf->bits)
    {
        case 1:
            print_fmt("32 bits\n");
            break;
        case 2:
            print_fmt("64 bits\n");
            break;
        default:
            print_fmt("Invalid value\n");
            return;
    }

    print_fmt("Endianness:\t");
    switch(elf->endian)
    {
        case 1:
            print_fmt("Little\n");
            break;
        case 2:
            print_fmt("Big\n");
            break;
        default:
            print_fmt("Invalid\n");
            return;
    }

    print_fmt("ABI:\t\t{xbyte}\n", &elf->abi);
    print_fmt("Type:\t\t{xshort}\n", &elf->type);
    print_fmt("Entry point:\t{xlong}\n", &elf->entry);
    print_fmt("Type:\t\t{xshort}\n", &elf->type);
    print_fmt("Machine:\t{xshort}\n", &elf->isa);

    print_fmt("\nReading sections:\n");
    for(uint32_t i = 0; i < elf->ph_num; i++)
    {
        elf_program_header_t* ph = (elf_program_header_t*)((uintptr_t)elf + (uintptr_t)elf->ph_off + (elf->ph_ent_size * i));

        switch(ph->type)
        {
            case ELF_PH_TYPE_NULL:
                print_fmt("\tNULL\t\t\t");
                break;
            case ELF_PH_TYPE_LOAD:
                print_fmt("\tLOAD\t\t\t");
                break;
            case ELF_PH_TYPE_DYNAMIC:
                print_fmt("\tDYNAMIC\t\t\t");
                break;
            case ELF_PH_TYPE_NOTE:
                print_fmt("\tNOTE\t\t\t");
                break;
            default:
                print_fmt("\tOTHER {xint}\t", &ph->type);
                break;
        }

        print_fmt("{xlong} => {xlong}\t", &ph->offset, &ph->vaddr);
        print_fmt("{long}\n", &ph->align);
        print_fmt("\t\t\t\t{xlong}    {xlong}\n\n", &ph->filesz, &ph->memsz);
    }
}

extern uintptr_t* vmm_pml_kernel;

uintptr_t load_elf(elf_header_t* elf, process_t* process)
{
    register_print_format("elf", print_elf_header);

    print_fmt("{elf}", elf);

    if((elf->magic[0] != ELF_MAGIC_0) || (elf->magic[1] != ELF_MAGIC_1) || (elf->magic[2] != ELF_MAGIC_2) || (elf->magic[3] != ELF_MAGIC_3))
        return 0;

    if(elf->version1 != 1 && elf->version2 != 1)
        return 0;

    if(elf->abi != ELF_ABI_SYSV)
        return 0;

    if(elf->bits != ELF_BITS_64)
        return 0;

    if(elf->isa != ELF_ISA_AMD64)
        return 0;

    uint64_t memsize = 0;

    #define ELF_PROG_HEADER(idx) ((elf_program_header_t*)((uintptr_t)elf + (uintptr_t)elf->ph_off + (elf->ph_ent_size * idx)))

    uintptr_t eof = 0;
    for(uint32_t i = 0; i < elf->ph_num; i++)
    {
        elf_program_header_t* ph = ELF_PROG_HEADER(i);
        if(ph->type != ELF_PH_TYPE_LOAD)
            continue;

        memsize += ph->memsz;

        if((ph->vaddr + ph->memsz) > eof)
            eof = (ph->vaddr + ph->memsz);
    }

    process->page_allocator = acquire_pool_allocator(PMM_PAGE_SIZE, ALIGN_UP(memsize + ELF_STACK_SIZE, PMM_PAGE_SIZE) + 2500 * PMM_PAGE_SIZE);
    // process->registers.cr3 = fetch_zero_pool(process->page_allocator);
    vmm_create_memmap(process->page_allocator, process->pcid, &process->registers.cr3);

    uintptr_t current_map = vmm_read_memmap();
    vmm_load_memmap(process->registers.cr3);

    for(uint32_t i = 0; i < elf->ph_num; i++)
    {
        elf_program_header_t* ph = ELF_PROG_HEADER(i);
        if(ph->type != ELF_PH_TYPE_LOAD)
            continue;

        // uint64_t flags = 0;
        // if(ph->flags & ELF_PH_FLAG_WRIT)
        //     flags |= VMM_FLAG_READ_WRITE;
        
        // if((ph->flags & ELF_PH_FLAG_EXEC) == 0)
        //     flags |= VMM_FLAG_XD;

        for(uintptr_t addr = ph->vaddr; addr < (ph->vaddr + ph->memsz); addr += PMM_PAGE_SIZE)
        {
            vmm_alloc(process->page_allocator, process->registers.cr3, process->pcid, addr, VMM_FLAG_READ_WRITE | VMM_FLAG_USER_PAGE);
        }

        uintptr_t ph_off = (uintptr_t)elf + ph->offset;
        memcpy((uint8_t*)ph_off, (uint8_t*)ph->vaddr, ph->filesz);
    }

    #undef ELF_PROG_HEADER

    uintptr_t stack_start = ALIGN_UP(eof, PMM_PAGE_SIZE);
    for(uintptr_t addr = stack_start; addr < stack_start + ELF_STACK_SIZE; addr += PMM_PAGE_SIZE)
    {
        vmm_alloc(process->page_allocator, process->registers.cr3, process->pcid, addr, VMM_FLAG_READ_WRITE | VMM_FLAG_USER_PAGE);
    }

    process->registers.rsp = stack_start + ELF_STACK_SIZE;
    process->registers.rip = elf->entry;
    vmm_load_memmap(current_map);

    return elf->entry;
}