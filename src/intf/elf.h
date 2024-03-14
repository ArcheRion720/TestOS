#pragma once
#include <stdint.h>
#include "scheduler/scheduler.h"

#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

#define ELF_ABI_SYSV 0x00

#define ELF_TYPE_NONE 0x00
#define ELF_TYPE_REL  0x01
#define ELF_TYPE_EXEC 0x02
#define ELF_TYPE_DYN  0x03
#define ELF_TYPE_CORE 0x04

#define ELF_ISA_X86   0x03
#define ELF_ISA_AMD64 0x3E

#define ELF_BITS_32   0x01
#define ELF_BITS_64   0x02

#define ELF_ENDIAN_L  0x01
#define ELF_ENDIAN_B  0x02

#define ELF_PH_TYPE_NULL    0x0
#define ELF_PH_TYPE_LOAD    0x1
#define ELF_PH_TYPE_DYNAMIC 0x2
#define ELF_PH_TYPE_INTERP  0x3
#define ELF_PH_TYPE_NOTE    0x4
#define ELF_PH_TYPE_SHLIB   0x5
#define ELF_PH_TYPE_PHDR    0x6
#define ELF_PH_TYPE_TLS     0x7
#define ELF_PH_TYPE_LOOS    0x60000000
#define ELF_PH_TYPE_HIOS    0x6FFFFFFF
#define ELF_PH_TYPE_LOPROC  0x70000000
#define ELF_PH_TYPE_HIPROC  0x7FFFFFFF

#define ELF_PH_FLAG_EXEC    0x1
#define ELF_PH_FLAG_WRIT    0x2
#define ELF_PH_FLAG_READ    0x4

#define ELF_SH_TYPE_NULL    0x0
#define ELF_SH_TYPE_PROGBIT 0x1
#define ELF_SH_TYPE_SYMTAB  0x2
#define ELF_SH_TYPE_STRTAB  0x3
#define ELF_SH_TYPE_RELA    0x4
#define ELF_SH_TYPE_HASH    0x5
#define ELF_SH_TYPE_DYNAMIC 0x6
#define ELF_SH_TYPE_NOTE    0x7
#define ELF_SH_TYPE_NOBITS  0x8
#define ELF_SH_TYPE_REL     0x9
#define ELF_SH_TYPE_DYNSYM  0x0B

#define ELF_SYM_BIND(info)  ((info) >> 4)
#define ELF_SYM_TYPE(info)  ((info) & 0xF)
#define ELF_SYM_GLOBAL(oth) ((oth) & 0x3)

#define ELF_SYM_BIND_LOCAL  0
#define ELF_SYM_BIND_GLOBAL 1
#define ELF_SYM_BIND_WEAK   2

#define ELF_SYM_TYPE_NOTYPE 0
#define ELF_SYM_TYPE_OBJECT 1
#define ELF_SYM_TYPE_FUNC   2
#define ELF_SYM_TYPE_SECT   3
#define ELF_SYM_TYPE_FILE   4
#define ELF_SYM_TYPE_COMMON 5
#define ELF_SYM_TYPE_TLS    6

#define ELF_STACK_SIZE      0x400000

struct elf_header
{
    uint8_t magic[4];
    uint8_t bits;
    uint8_t endian;
    uint8_t version1;
    uint8_t abi;
    uint8_t abi_ver;
    uint8_t padding[7];
    uint16_t type;
    uint16_t isa;
    uint32_t version2;
    uint64_t entry;
    uint64_t ph_off;
    uint64_t sh_off;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_ent_size;
    uint16_t ph_num;
    uint16_t sh_ent_size;
    uint16_t sh_num;
    uint16_t sh_str_idx;
};
typedef struct elf_header elf_header_t;

struct elf_program_header
{
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};
typedef struct elf_program_header elf_program_header_t;

struct elf_section_header
{
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addr_align;
    uint64_t entry_size;
};
typedef struct elf_section_header elf_section_header_t;

struct elf_symbol
{
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
};
typedef struct elf_symbol elf_symbol_t;

struct elf_relocation_rel
{

};

struct elf_relocation_rela
{
    
};

uintptr_t load_elf(elf_header_t* elf, process_t* pml4);
uint8_t* elf_resolve_func_symbol(elf_header_t* elf, uintptr_t sym_addr);