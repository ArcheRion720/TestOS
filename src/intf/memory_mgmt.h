#pragma once
#include "memory/pmm.h"
#include "memory/pool_allocator.h"
#include "memory/vmm.h"

extern uintptr_t hhdm; 
extern struct pool_allocator* pmm_kernel_allocator;

#define HH_ADDR(x) ((uintptr_t)(x) | hhdm)
#define LH_ADDR(x) ((uintptr_t)(x) & (~hhdm))

void init_memory_manager();