#pragma once
#include "common.h"
#include "string.h"
#include "rng.h"

#define MAX_TASKS 256

struct task
{
    uint64_t id;
    uint64_t state;
    uint32_t priority;
    string_t name;
    registers_t regs;
};
typedef struct task task_t;

struct scheduler_state
{
    task_t* idle;
    task_t* tasks;
    uint64_t count;
    uint64_t current;

    // Lottery Scheduling
    rng_state_t lottery;
    uint32_t tickets;
};
typedef struct scheduler_state scheduler_state_t;

void init_tasking();
void start_tasking();
void start_task(string_t name, uintptr_t ptr, uintptr_t cr3, bool_t user);
void setup_scheduler(registers_t* regs);
void schedule(registers_t* regs);
task_t* get_current_task();
task_t* get_task(uint64_t index);