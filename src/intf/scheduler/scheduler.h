#pragma once
#include <stdint.h>
#include "linked_list.h"
#include "scheduler/rng.h"
#include "memory/pmm.h"
#include "registers.h"

struct process
{
    struct link* prev, next;
    uint64_t priority;
    uint64_t quants;
    uint16_t pcid;
    registers_t registers;
    pool_allocator_t* page_allocator;
};
typedef struct process process_t;

struct scheduler_state
{
    process_t* current;
    uint64_t tickets;
    uint64_t tasks_count;
    rng_state_t rng_state;
};
typedef struct scheduler_state scheduler_state_t;

process_t* process_obj_create();
void schedule_process(process_t* process);
void scheduler_handler(registers_t*);
void start_scheduling();