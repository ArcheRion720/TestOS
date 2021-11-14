#include "memory/pmm.h"
#include "bootloader.h"

static struct memory_entry_header* head;
static uint64_t memory_block_count;

void clear_page(uintptr_t addr)
{
    uint64_t* ptr = (uint64_t*)addr;
    for(int i = 0; i < 0x40; i++)
    {
        *ptr = 0;
        ptr++;
    }
}

void* alloc_block_single()
{
    uintptr_t addr = (uintptr_t)head;
    head = head->next;

    clear_page(addr);
    //k_memset(addr, 0, 0x1000);

    return (void*)addr;
}

void* alloc_block_adjacent(uint64_t count)
{
    uint64_t temp = 0;
    struct memory_entry_header* patch = NULL;
    struct memory_entry_header* current = head;
    struct memory_entry_header* next = current->next;

    for(;;)
    {
        if(((uintptr_t)next) - ((uintptr_t)current) == PAGE_SIZE)
        {
            temp++;
        }
        else
        {
            patch = current;
            temp = 0;
        }

        if(temp == count)
        {
            void* result;
            if(patch != NULL)
            {
                result = patch->next;
                patch->next = next->next;
            }
            else
            {
                result = head;
                head = next->next;
            }

            for(uint64_t i = 0; i < count; i++)
            {
                clear_page((uintptr_t)result + (uintptr_t)(i * 0x1000));
            }

            return result;
        }

        current = next;
        next = next->next;
    }
}

void init_memory_manager(struct stivale2_struct* stivale)
{
    struct stivale2_struct_tag_memmap* memory_map = stivale2_get_tag(stivale, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_mmap_entry* entry;

    struct memory_entry_header* current_mem_entry = NULL;
    struct memory_entry_header* last_mem_entry = NULL;

    //Prepare and link memory page headers

    for(uint64_t i = 0; i < memory_map->entries; i++)
    {
        entry = &memory_map->memmap[i];

        if(entry->type != STIVALE2_MMAP_USABLE)
            continue;

        for(uintptr_t ptr = entry->base; ptr < (entry->base + entry->length); ptr += PAGE_SIZE)
        {
            current_mem_entry = (struct memory_entry_header*)ptr;
            *current_mem_entry = (struct memory_entry_header){ };

            if(last_mem_entry == NULL)
            {
                head = (struct memory_entry_header*)HIGHER_HALF((uintptr_t)current_mem_entry);
            }
            else
            {
                last_mem_entry->next = (struct memory_entry_header*)HIGHER_HALF((uintptr_t)current_mem_entry);
            }

            last_mem_entry = (struct memory_entry_header*)HIGHER_HALF((uintptr_t)current_mem_entry);

            memory_block_count++;
        }
    }

    debug_write(TEXT("Initialised physical memory\n"));
}

uint32_t getUsableMemoryRegionCount()
{
    return memory_block_count;
}

void free(uintptr_t ptr)
{
    *((struct memory_entry_header*)ptr) = (struct memory_entry_header)
    {
        .next = head
    };

    head = (struct memory_entry_header*)ptr;
}

void k_memset(void* addr, uint8_t value, uint64_t size)
{
    uint8_t* ptr = (uint8_t*)addr;
    for(uint64_t i = 0; i < size; i++)
    {
        *ptr = value;
        ptr++;
    }
}

uintptr_t get_current_head()
{
    return (uintptr_t)head;
}