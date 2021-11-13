#include "bootloader.h"

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_header =
{
    .tag = 
    {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },

    .flags = 0
};

static struct stivale2_struct_tag_memmap memory_map_tag =
{
    .tag = 
    {
        .identifier = STIVALE2_STRUCT_TAG_MEMMAP_ID,
        .next = (uint64_t)&terminal_header
    }
};

static struct stivale2_struct_tag_framebuffer framebuffer_tag = 
{
    .tag = 
    {
        .identifier = STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&memory_map_tag
    },
};

static struct stivale2_header_tag_framebuffer framebuffer_header = 
{
    .tag = 
    {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t) &framebuffer_tag
    },

    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_header =
{
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2),
    .tags = (uintptr_t)&framebuffer_header
};

void* stivale2_get_tag(struct stivale2_struct* stivale, uint64_t id)
{
    struct stivale2_tag* tag = (void*)stivale->tags;
    for(;;)
    {
        if(tag == NULL)
            return NULL;

        if(tag->identifier == id)
            return tag;

        tag = (void*)tag->next;
    }
}