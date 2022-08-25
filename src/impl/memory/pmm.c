#include "memory/pmm.h"
#include "utils.h"
#include "common.h"
#include "sync.h"

uint16_t region_count;
memory_region_t regions[1024];

void init_memory_manager(struct stivale2_struct* stivale)
{
    struct stivale2_struct_tag_memmap* memory_map = stivale2_get_tag(stivale, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    struct stivale2_mmap_entry* entry;

    //Hope will never happen
    //if(memory_map->entries > region_count)
    //{
    //  Some creative fix
    //}

    for(uint64_t i = 0; i < memory_map->entries; i++)
    {
        entry = &memory_map->memmap[i];
        if(entry->type != STIVALE2_MMAP_USABLE)
            continue;

        uint64_t blocks = entry->length / PMM_HBLOCK_SIZE;
        uint64_t bmap_blocks = (PMM_ENTRY_BITS * blocks) / 8;
        bmap_blocks = ALIGN_UP(bmap_blocks, PMM_HBLOCK_SIZE); //(bmap_blocks + 0x1FFF) & ~0x1FFF;
        bmap_blocks /= PMM_HBLOCK_SIZE;
        blocks -= bmap_blocks;

        if(blocks == 0)
            continue;

        regions[region_count].base = entry->base;
        regions[region_count].alloc_base = entry->base + (bmap_blocks * PMM_HBLOCK_SIZE);
        regions[region_count].blocks = blocks;
        regions[region_count].bmap_reserved_blocks = bmap_blocks;

        for(uint8_t k = 0; k < 8; k++)
        {
            regions[region_count].offsets[k] = ((1 << k) - 1) * blocks;
        }

        region_count++;
    }

    ticket_lock_init(MEMORY_ALLOC_LOCK);
}

pmm_bitmap_iterator_t iterate_bitmap(uint64_t order)
{
    pmm_bitmap_iterator_t result;

    uint64_t block_count;
    uint64_t limit_min;
    uint64_t limit_max;

    uint64_t offset_min = (1 << order) - 1;
    uint64_t offset_max = (1 << (order + 1)) - 1;
    for(uint16_t i = 0; i < region_count; i++)
    {
        block_count = regions[i].blocks;
        limit_min = block_count * offset_min;
        limit_max = block_count * offset_max;
        for(uint64_t bit = limit_min; bit < limit_max; bit++)
        {
            // Can be optimised
            if(!TEST_BIT((uint8_t*)regions[i].base, bit))
            {
                result.valid = 1;
                result.region = i;
                result.abs_bit = bit;
                result.rel_bit = bit - limit_min;

                return result;
            }
        }
    }

    result.valid = 0;
    return result;
}

pmm_bitmap_iterator_t iterate_bitmap_top(uint64_t blocks)
{
    pmm_bitmap_iterator_t result;
    uint64_t conseq = 0;
    uint64_t start_bit;

    for(uint16_t i = 0; i < region_count; i++)
    {
        start_bit = 0;
        conseq = 0;
        for(uint64_t bit = 0; bit < regions[i].blocks; i++)
        {
            if(TEST_BIT((uint8_t*)regions[i].base, bit))
            {
                conseq = 0;
            }
            else
            {
                if(conseq == 0)
                    start_bit = bit;

                conseq++;
            }

            if(conseq == blocks)
            {
                result.valid = 1;
                result.region = i;
                result.abs_bit = start_bit;
                result.rel_bit = start_bit;

                return result;
            }
        }
    }

    result.valid = 0;
    return result;
}

void set_bits_lower_order(memory_region_t* region, uint64_t order, uint64_t bit)
{
    if(order >= PMM_LOWEST_ORD)
        return;

    SET_BIT((uint8_t*)region->base, (region->offsets[order + 1] + 2 * bit    ));
    SET_BIT((uint8_t*)region->base, (region->offsets[order + 1] + 2 * bit + 1));

    set_bits_lower_order(region, order + 1, 2 * bit);
    set_bits_lower_order(region, order + 1, 2 * bit + 1);
}

void clear_bits_lower_order(memory_region_t* region, uint64_t order, uint64_t bit)
{
    if(order >= PMM_LOWEST_ORD)
        return;

    CLEAR_BIT((uint8_t*)region->base, (region->offsets[order + 1] + 2 * bit    ));
    CLEAR_BIT((uint8_t*)region->base, (region->offsets[order + 1] + 2 * bit + 1));

    clear_bits_lower_order(region, order + 1, 2 * bit);
    clear_bits_lower_order(region, order + 1, 2 * bit + 1);
}

void* malloc_span(uint64_t blocks)
{
    pmm_bitmap_iterator_t it = iterate_bitmap_top(blocks);

    for(uint64_t bit = it.rel_bit; bit < it.rel_bit + blocks; bit++)
    {
        SET_BIT((uint8_t*)regions[it.region].base, bit);
        set_bits_lower_order(&regions[it.region], 0, bit);
    }

    return (uint8_t*)regions[it.region].alloc_base + (it.rel_bit * PMM_HBLOCK_SIZE);
}

void* malloc(uint64_t size)
{
    ticket_lock_acquire(MEMORY_ALLOC_LOCK);
    if(size == 0)
        return 0;

    if(size > PMM_HBLOCK_SIZE)
    {
        void* result = malloc_span(ALIGN_UP(size, PMM_HBLOCK_SIZE) / PMM_HBLOCK_SIZE);
        ticket_lock_release(MEMORY_ALLOC_LOCK);
        return result;
    }

    size--;
    uint64_t order;
    asm("bsr %1, %0" : "=r"(order) : "r"(size));
    order = 12 - order;

    pmm_bitmap_iterator_t it = iterate_bitmap(order);
    if(!it.valid)
    {
        ticket_lock_release(MEMORY_ALLOC_LOCK);
        return 0;
    }

    uint8_t* base = (uint8_t*)regions[it.region].base;
    uint64_t blocks = regions[it.region].blocks;

    SET_BIT(base, it.abs_bit);

    uint64_t itbit = it.rel_bit;
    for(uint64_t o = order; o > 0; o--)
    {
        itbit /= 2;
        uint64_t b = regions[it.region].offsets[o - 1] + itbit;
        SET_BIT(base, b);
    }

    set_bits_lower_order(&regions[it.region], order, it.rel_bit);
    ticket_lock_release(MEMORY_ALLOC_LOCK);
    return (uint8_t*)regions[it.region].alloc_base + (PMM_HBLOCK_SIZE / (1 << order) * it.rel_bit);
}

void free_span(void* ptr, uint64_t blocks)
{
    uintptr_t addr = (uintptr_t)ptr;
    uint16_t reg;
    for(reg = region_count; reg > 0; reg--)
    {
        if(regions[reg - 1].alloc_base < addr)
        {
            reg--;
            break;
        }
    }

    uint64_t bit = (addr - regions[reg].alloc_base) / PMM_HBLOCK_SIZE;
    for(uint64_t i = 0; i < blocks; i++)
    {
        CLEAR_BIT((uint8_t*)regions[reg].base, bit);
        clear_bits_lower_order(&regions[reg], 0, bit);
    }
}

void free(void* ptr, uint64_t size)
{
    ticket_lock_acquire(MEMORY_ALLOC_LOCK);
    if(size > PMM_HBLOCK_SIZE)
    {
        free_span(ptr, ALIGN_UP(size, PMM_HBLOCK_SIZE) / PMM_HBLOCK_SIZE);
        ticket_lock_release(MEMORY_ALLOC_LOCK);
        return;
    }

    uintptr_t addr = (uintptr_t)ptr;

    size--;
    uint64_t order;
    asm("bsr %1, %0" : "=r"(order) : "r"(size));
    order = 12 - order;

    uint16_t reg;
    for(reg = region_count; reg > 0; reg--)
    {
        if(regions[reg - 1].alloc_base < addr)
        {
            reg--;
            break;
        }
    }

    uint64_t blocks = regions[reg].blocks;
    uint64_t bit = ((addr - regions[reg].alloc_base) * (1 << order)) / PMM_HBLOCK_SIZE;

    CLEAR_BIT((uint8_t*)regions[reg].base, regions[reg].offsets[order] + bit);
    clear_bits_lower_order(&regions[reg], order, bit);

    uint64_t ibit = bit;
    for(uint64_t i = order; i > 0; i--)
    {
        ibit ^= 1;
        //Check buddy slot
        if(TEST_BIT((uint8_t*)regions[reg].base, ibit))
            break;
        
        //Merge
        ibit /= 2;
        CLEAR_BIT((uint8_t*)regions[reg - 1].base, ibit);
    }
    ticket_lock_release(MEMORY_ALLOC_LOCK);
}

void* malloc_page()
{
    return malloc(PAGE_SIZE);
}

void free_page(void* ptr)
{
    return free(ptr, PAGE_SIZE);
}

malloc_report_t malloc_ex(uint64_t size)
{
    malloc_report_t report;
    if(size > PMM_HBLOCK_SIZE)
    {
        report.size = (ALIGN_UP(size, PMM_HBLOCK_SIZE));
        report.data = malloc(size);
        return report;
    }

    size--;
    uint64_t order;
    asm("bsr %1, %0" : "=r"(order) : "r"(size));
    order = 12 - order;

    report.size = PMM_HBLOCK_SIZE / (1 << order);
    report.data = malloc(size);

    return report;
}