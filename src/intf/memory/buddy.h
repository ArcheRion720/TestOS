#pragma once
#include <stdint.h>
#include <stddef.h>

struct buddy_head
{
    uint16_t status;
    uint16_t level;
};

struct buddy_head* buddy_create();
void* buddy_alloc(struct buddy_head* head, size_t size);
void buddy_free(void* ptr);