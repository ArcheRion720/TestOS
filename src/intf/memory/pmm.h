#pragma once
#include "bootloader.h"

#define PAGE_SIZE 0x1000
#define HIGHER_HALF(x) ((x) + 0xffff800000000000)

struct memory_entry_header
{
    struct memory_entry_header* next;
};

struct memory_entry
{
    union
    {
        struct memory_entry_header header;
        uint8_t data[PAGE_SIZE];
    };
};

void* alloc_block_single();
void* alloc_block_adjacent(uint64_t count);
void init_memory_manager(struct stivale2_struct* stivale);

uint32_t getUsableMemoryRegionCount();
void k_memset(void* addr, uint8_t value, uint64_t size);
uintptr_t get_current_head();