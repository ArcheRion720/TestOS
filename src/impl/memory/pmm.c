#include <stddef.h>
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "utils.h"
#include "limine.h"
#include "utils.h"
#include "print.h"

extern struct limine_memmap_request mmap_request;
extern struct limine_hhdm_request hhdm_request;

static uint64_t usable_memory = 0;
static memory_area_ptr memory_head = 0;
static pool_allocator_t allocators;

void print_mem_entry(struct limine_memmap_entry* e)
{
    print_fmt("Base: {xlong}\tLength: {xlong}\tType: {long}", &e->base, &e->length, &e->type);
}

void init_memory_manager()
{
    register_print_format("mem", print_mem_entry);

    if(mmap_request.response == NULL || mmap_request.response->entry_count == 0)
        return;

    memory_area_ptr current = 0;
    memory_area_ptr previous = 0;

    struct limine_memmap_entry* entry;
    for(uint64_t i = 0; i < mmap_request.response->entry_count; i++)
    {
        entry = mmap_request.response->entries[i];

        print_fmt("{mem}\n", entry);

        if(entry->type != LIMINE_MEMMAP_USABLE)
            continue;

        if(SIZE_IN_PAGES(entry->length) < 2)
            continue;

        current = (memory_area_ptr)entry->base;
        current->pages_total = SIZE_IN_PAGES(entry->length) - 1;
        current->pages_used = 0;
        current->block_count = 1;
        current->blocks[0].size = current->pages_total;
        current->blocks[0].used = PMM_BLOCK_FREE;

        if(previous)
            previous->next = HH_ADDR(current);
        else
            memory_head = HH_ADDR(current);

        usable_memory += (current->pages_total * PMM_PAGE_SIZE);
        previous = current;
    }

    pool_allocator_create(&allocators, sizeof(pool_allocator_t), PMM_PAGE_SIZE);
}

void* malloc(uint64_t size)
{
    uint32_t req_pages = SIZE_IN_PAGES(ALIGN_UP(size, PMM_PAGE_SIZE));
    uint32_t pages_offset;

    for(memory_area_ptr ma = memory_head; LH_ADDR(ma); ma = ma->next)
    {
        pages_offset = 1;
        for(uint32_t i = 0; i < ma->block_count; i++)
        {
            if(ma->blocks[i].used || ma->blocks[i].size < req_pages)
            {
                pages_offset += ma->blocks[i].size;
                continue;
            }

            uint32_t remainder = ma->blocks[i].size - req_pages;
            ma->blocks[i].used = PMM_BLOCK_USED;

            if(remainder != 0)
            {
                if(ma->block_count != PMM_BLOCKS_LIMIT)
                {
                    ma->block_count++;
                    ma->blocks[i].size = req_pages;
                    if(i < ma->block_count - 1)
                    {
                        memmove(
                            &ma->blocks[i], 
                            &ma->blocks[i + 1], 
                            (ma->block_count - i - 1) * sizeof(struct memory_block));
                    }
                    ma->blocks[i + 1].used = PMM_BLOCK_FREE;
                    ma->blocks[i + 1].size = remainder;
                }
            }

            return (void*)((uintptr_t)LH_ADDR(ma) + (pages_offset * PMM_PAGE_SIZE));
        }
    }

    return 0;
}

uint8_t ptr_in_area_range(memory_area_ptr ma, uintptr_t addr)
{
    return (addr > (uintptr_t)ma) && (addr < ((uintptr_t)ma + (ma->pages_total + 1) * PMM_PAGE_SIZE));
}

#define PMM_MERGE_NON 0x0
#define PMM_MERGE_BLL 0x1
#define PMM_MERGE_BLR 0x2

void free(void* ptr)
{
    if(((uintptr_t)ptr % PMM_PAGE_SIZE) != 0) 
        return;

    for(memory_area_ptr ma = memory_head; LH_ADDR(ma); ma = ma->next)
    {
        if(!ptr_in_area_range(LH_ADDR(ma), (uintptr_t)ptr))
            continue;

        uint32_t blk_idx = 0;
        uintptr_t addr = (uintptr_t)ma + PMM_PAGE_SIZE;
        while(addr != (uintptr_t)ptr)
        {
            addr += PMM_PAGE_SIZE * ma->blocks[blk_idx].size;
            blk_idx++;
        }

        uint8_t mem_op = PMM_MERGE_NON;
        if(blk_idx != 0 && ma->blocks[blk_idx - 1].used == PMM_BLOCK_FREE)
            mem_op |= PMM_MERGE_BLL;

        if(blk_idx != ma->block_count - 1 && ma->blocks[blk_idx + 1].used == PMM_BLOCK_FREE)
            mem_op |= PMM_MERGE_BLR;

        switch(mem_op)
        {
            case PMM_MERGE_BLL:
                ma->blocks[blk_idx - 1].size += ma->blocks[blk_idx].size;
                ROTL(ma->blocks, blk_idx, 1, ma->block_count, struct memory_block);
                ma->block_count--;
                return;
            case PMM_MERGE_BLR:
                ma->blocks[blk_idx].size += ma->blocks[blk_idx + 1].size;
                ma->blocks[blk_idx].used = PMM_BLOCK_FREE;
                ROTL(ma->blocks, blk_idx + 1, 1, ma->block_count, struct memory_block);
                ma->block_count--;
                return;
            case (PMM_MERGE_BLL | PMM_MERGE_BLR):
                ma->blocks[blk_idx - 1].size += ma->blocks[blk_idx].size + ma->blocks[blk_idx + 1].size;
                ROTL(ma->blocks, blk_idx, 2, ma->block_count, struct memory_block);
                ma->block_count -= 2;
                return;
            case PMM_MERGE_NON:
                ma->blocks[blk_idx].used = PMM_BLOCK_FREE;
                return;
            default:  //should never happen
                return;
        }
    }
}

void pool_allocator_create(pool_allocator_t* allocator, uint32_t item_size, uint32_t mem_size)
{
    void* block = HH_ADDR(malloc(mem_size));
    
    item_size = ALIGN_UP(item_size, 8);
    uint32_t items_count = mem_size / item_size;

    allocator->items.next = &allocator->items;
    allocator->items.prev = &allocator->items;

    // *((struct link*)block) = LINKED_LIST(((struct link*)block)[0]);
    for(uint32_t i = 0; i < items_count; i++)
    {
        struct link* item = (struct link*)((uintptr_t)block + (i * item_size));
        linked_list_add_forward(item, &allocator->items);
    }

    allocator->size = items_count;
    allocator->used = 0;
    allocator->item_size = item_size;
}

pool_allocator_t* pool_allocator_acquire(uint32_t item_size, uint32_t mem_size)
{
    pool_allocator_t* result = (pool_allocator_t*)HH_ADDR(pool_fetch(&allocators));

    if(result == 0)
        return 0;

    pool_allocator_create(result, item_size, mem_size);
    return result;
}

void pool_allocator_dispose(pool_allocator_t* allocator)
{
    pool_drop(&allocators, allocator);
}

void* pool_fetch(pool_allocator_t* alloc)
{
    if(alloc->used == alloc->size)
    {
        __builtin_trap();
    }

    struct link* it;
    linked_list_foreach_link(it, &alloc->items)
    {
        linked_list_remove(it);
        alloc->used++;
        return LH_ADDR(it);
    }


    __builtin_trap();
    return 0;
}

void* pool_fetch_zero(pool_allocator_t* alloc)
{
    void* ptr = pool_fetch(alloc);
    memset(HH_ADDR(ptr), 0, alloc->item_size);
    return ptr;
}

void pool_drop(pool_allocator_t* alloc, void* item)
{
    linked_list_add_forward(&item, &alloc->items);
}