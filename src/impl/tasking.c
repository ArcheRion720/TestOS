#include "tasking.h"
#include "memory/pmm.h"
#include "interrupts.h"
#include "common.h"
#include "utils.h"
#include "sync.h"
#include "rng.h"

#include <stdint.h>

scheduler_state_t scheduler = {0};

void init_tasking()
{
    scheduler.idle = malloc(sizeof(task_t));
    scheduler.tasks = malloc(sizeof(task_t) * MAX_TASKS);
    memset(scheduler.tasks, 0, sizeof(task_t) * MAX_TASKS);

    scheduler.count = 0;

    scheduler.idle->name = cstr("idle");
    scheduler.idle->id = 1;
    scheduler.idle->state = 0;
}

void start_tasking()
{
    register_isr_handler(32, &setup_scheduler);
}

void start_task(string_t name, uintptr_t ptr, uintptr_t cr3, bool_t user) 
{
    task_t* task = &scheduler.tasks[scheduler.count];
    task->name = name;
    task->state = 0;
    task->id = scheduler.count + 1;
    task->regs.rip = ptr;

    task->priority = 100;
    scheduler.tickets += 100;
    
    task->regs.cr3 = cr3;

    ///TODO: USER process
    // if(user)
    // {
    //     task->regs.cs = 0x38;
    //     task->regs.ds = 0x40;
    // }
    // else
    {
        task->regs.cs = 0x28;
        task->regs.ds = 0x10;
    }

    task->regs.es = 0x10;
    task->regs.fs = 0x10;
    task->regs.gs = 0x10;
    task->regs.rflags = (1 << 9);

    uintptr_t stack = malloc(8192);
    memset(stack, 0, 8192);
    task->regs.rsp = stack + 8192;

    log("Task added!");

    scheduler.count++;
}

void setup_scheduler(registers_t* regs)
{
    if(scheduler.count == 0)
        return;

    scheduler.lottery = 7675;
    register_isr_handler(32, &schedule);

    task_t* next = &scheduler.tasks[0];
    *regs = next->regs;

    return;
}

void schedule(registers_t* regs)
{    
    if(scheduler.count == 0)
    {
        ///TODO: PANIC as no processes are operable!
        //warn("Returning from schedule() early!");
        return;
    }
    
    task_t* current = &scheduler.tasks[scheduler.current];

    uint32_t ticket = rng_next(&scheduler.lottery, 0, scheduler.tickets);

    for(uint32_t i = 0; i < scheduler.count; i++)
    {
        if(ticket > scheduler.tasks[i].priority)
        {
            ticket -= scheduler.tasks[i].priority;
        }
        else
        {
            if(i == scheduler.current)
                return;

            current->regs = *regs;
            *regs = scheduler.tasks[i].regs;
            scheduler.current = i;
            return;
        }
    }

    printf("PANIC!");
    asm("hlt");
    /// TODO: PANIC

    return;
}

task_t* get_task(uint64_t index)
{
    return &scheduler.tasks[index];
}

task_t* get_current_task()
{
    if(scheduler.count == 0)
        return (uintptr_t)0;

    return &scheduler.tasks[scheduler.current];
}
