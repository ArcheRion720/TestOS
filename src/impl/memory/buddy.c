#include "memory_mgmt.h"

#define BUDDY_LEVELS    8
#define BUDDY_LEVEL_MIN 5

#define BUDDY_BLOCK_FREE    0
#define BUDDY_BLOCK_TAKEN   1

#define BUDDY_ARENA_SIZE    PMM_PAGE_SIZE

struct buddy_head* pmm_kernel_heap;

struct buddy_head* buddy_create()
{
    struct buddy_head* block = (struct buddy_head*)HH_ADDR(malloc(BUDDY_ARENA_SIZE));
    block->level = BUDDY_LEVELS - 1;
    block->status = BUDDY_BLOCK_FREE;

    return block;
}

struct buddy_head* buddy_get_buddy(struct buddy_head* block)
{
    uintptr_t ptr = (uintptr_t)block;
    uintptr_t mask = 1ull << (block->level + BUDDY_LEVEL_MIN);

    return (struct buddy_head*)(ptr ^ mask);
}

struct buddy_head* buddy_split(struct buddy_head* block)
{
    if(block->status == BUDDY_BLOCK_TAKEN)
        __builtin_trap();

    uintptr_t ptr = (uintptr_t)block;
    uintptr_t mask = 1ull << (block->level - 1 + BUDDY_LEVEL_MIN);

    block->level -= 1;

    struct buddy_head* split = (struct buddy_head*)(ptr | mask);
    split->level = block->level;
    split->status = BUDDY_BLOCK_FREE;

    return split;
}

struct buddy_head* buddy_merge(struct buddy_head* block)
{
    struct buddy_head* buddy = buddy_get_buddy(block);
    if(buddy->level != block->level)
        return 0;

    if((block->status != BUDDY_BLOCK_FREE) || (block->status != buddy->status))
        return 0;

    uintptr_t ptr = (uintptr_t)block;
    uintptr_t mask = (uintptr_t)(-1ull) << (1 + block->level + BUDDY_LEVEL_MIN);

    struct buddy_head* merged_block = (struct buddy_head*)(ptr & mask);
    merged_block->level = block->level + 1;

    return merged_block;
}

uint16_t buddy_level(size_t size)
{
    size += sizeof(struct buddy_head);

    uint16_t level = 0;
    uint16_t mask = 1 << BUDDY_LEVEL_MIN;
    while(size > mask)
    {
        mask <<= 1;
        level++;
    }

    return level;
}

struct buddy_head* buddy_find(struct buddy_head* head, uint16_t level)
{
    const uintptr_t offset = (uintptr_t)head;

    struct buddy_head* current;
    for(uintptr_t ptr = offset; ptr < offset + BUDDY_ARENA_SIZE; ptr += (1ull << (current->level + BUDDY_LEVEL_MIN)))
    {
        current = (struct buddy_head*)ptr;
        if((current->level >= level) && (current->status == BUDDY_BLOCK_FREE))
        {
            return current;
        }
    }

    return 0;
}

void* buddy_alloc(struct buddy_head* head, size_t size)
{
    uint16_t level = buddy_level(size);
    struct buddy_head* node = buddy_find(head, level);

    if(!node)
        return 0;

    while(node->level > level)
    {
        node = buddy_split(node);
    }

    node->status = BUDDY_BLOCK_TAKEN;
    return (void*)((uintptr_t)node + sizeof(struct buddy_head));
}

void buddy_free(void* ptr)
{
    struct buddy_head* node = (struct buddy_head*)((uintptr_t)ptr - sizeof(struct buddy_head));

    node->status = BUDDY_BLOCK_FREE;

    for (uint16_t level = node->level; level < BUDDY_LEVELS; level++)
    {
        struct buddy_head* buddy = buddy_get_buddy(node);
        if (node->level == buddy->level && buddy->status == BUDDY_BLOCK_FREE)
        {
            node = buddy_merge(node);
            continue;
        }

        break;
    }

    node->status = BUDDY_BLOCK_FREE;
}