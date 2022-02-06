#pragma once
#include "common.h"
#include "string.h"

#define MAX_TASKS 256

struct task
{
    uint64_t id;
    uint64_t state;
    uint32_t critical;
    string_t name;
    registers_t regs;
};
typedef struct task task_t;

void init_tasking();
void start_task(string_t name, uintptr_t ptr, uintptr_t cr3);
void schedule(registers_t* regs);
task_t* get_current_task();
void enter_critical_section();
void leave_critical_section();
uint32_t get_critical();

task_t* get_task(uint64_t index);