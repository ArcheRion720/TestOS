#include "tasking.h"
#include "memory/pmm.h"
#include "interrupts.h"
#include "common.h"
#include "utils.h"
#include "sync.h"

#include <stdint.h>

task_t* queue;
uint64_t task_count;
uint64_t current_task;
uint32_t critical;

void init_tasking()
{
    queue = malloc(sizeof(task_t) * MAX_TASKS);
    memset(queue, 0, sizeof(task_t) * MAX_TASKS);

    task_count = 1;

    queue[0].name = cstr("kernel");
    queue[0].id = 1;
    queue[0].state = 0;

    ticket_lock_init(FRAMEBUFFER_LOCK);

    register_isr_handler(32, &schedule);
}

void start_task(string_t name, uintptr_t ptr, uintptr_t cr3) 
{
    queue[task_count].name = name;
    queue[task_count].state = 0;
    queue[task_count].id = task_count + 1;
    queue[task_count].regs.rip = ptr;
    
    queue[task_count].regs.cr3 = cr3;
    queue[task_count].regs.cs = 0x28;
    queue[task_count].regs.ds = 0x10;
    queue[task_count].regs.es = 0x10;
    queue[task_count].regs.fs = 0x10;
    queue[task_count].regs.gs = 0x10;
    queue[task_count].regs.rflags = (1 << 9);

    uintptr_t stack = malloc(8192);
    memset(stack, 0, 8192);
    queue[task_count].regs.rsp = stack + 8192;

    log("Task added!");

    task_count++;
}

void schedule(registers_t* regs)
{    
    //log("Schedule called!");

    if(task_count < 2 || queue[current_task].critical)
    {
        //warn("Returning from schedule() early!");
        return;
    }
    
    task_t* current = &queue[current_task];

    current_task++;

    if(current_task == task_count)
        current_task = 1;

    task_t* next = &queue[current_task];

    current->regs = *regs;
    *regs = next->regs;

    return;
}

task_t* get_task(uint64_t index)
{
    return &queue[index + 1];
}

task_t* get_current_task()
{
    if(task_count == 0)
        return (uintptr_t)0;

    return &queue[current_task];
}
