#include "memory_mgmt.h"
#include "utils.h"

#define PMM_BLOCKS_LIMIT ((PMM_PAGE_SIZE - sizeof(struct memory_area)) / (sizeof(struct memory_block)))

memory_area_ptr memory_head = 0;

void* malloc(uint64_t size)
{
    uint32_t req_pages = PMM_SIZE_PAGES(ALIGN_UP(size, PMM_PAGE_SIZE));
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
