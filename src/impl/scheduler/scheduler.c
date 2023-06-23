#include "scheduler/scheduler.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "linked_list.h"
#include "interrupts.h"
#include "gdt.h"
#include "capabilities.h"

#include "print.h"

static pool_allocator_t* task_allocator;
static linked_list_t tasks;
static scheduler_state_t scheduler;

void idle_task_handler();
static uint8_t idle_task_stack[512];

void init_scheduler()
{
    task_allocator = acquire_pool_allocator(sizeof(process_t), sizeof(process_t) * 200);
    scheduler.rng_state = 9223372036854775810ULL;
    
    tasks = LINKED_LIST(tasks);
    
    process_t* idle_task = (process_t*)HH_ADDR(fetch_zero_pool(task_allocator));
    *((linked_list_t*)idle_task) = LINKED_LIST(*idle_task);
    idle_task->priority = 0;
    idle_task->registers.cs = GDT_SEGMENT_OFFSET(GDT_CS0_64);
    idle_task->registers.ds = GDT_SEGMENT_OFFSET(GDT_DS0_64);
    idle_task->registers.es = GDT_SEGMENT_OFFSET(GDT_DS0_64);
    idle_task->registers.fs = GDT_SEGMENT_OFFSET(GDT_DS0_64);
    idle_task->registers.gs = GDT_SEGMENT_OFFSET(GDT_DS0_64);
    idle_task->registers.rflags = REG_FLAG_IF;

    idle_task->registers.rip = &idle_task_handler;
    idle_task->registers.rsp = (uintptr_t)(&idle_task_stack) + sizeof(idle_task_stack);
    idle_task->page_allocator = 0;
    
    add_link_back(&tasks, (struct link*)idle_task);
}

void scheduler_start()
{
    register_isr_handler(32, (isr_t)&scheduler_handler);
}

process_t* get_next_task()
{
    uint64_t ticket = rng_next(&scheduler.rng_state, 0, scheduler.tickets);
    struct link* it;
    for_each_link(it, &tasks)
    {
        if(ticket <= ((process_t*)it)->priority)
            break;
        
        ticket -= ((process_t*)it)->priority;
    }

    return (process_t*)it;
}

void scheduler_handler(registers_t* regs)
{
    print_fmt("Handler run!\n");
    process_t* next = get_next_task();

    if(scheduler.current == 0)
    {
        scheduler.current = next;
        *regs = next->registers;
        return;
    }

    if(scheduler.current->pcid == next->pcid)
        return;

    vmm_invalidate_desc_t inv_desc = {
        .addr = 0,
        .pcid = scheduler.current->pcid
    };

    vmm_invalidate(inv_desc);

    scheduler.current->registers = *regs;
    *regs = next->registers;

    scheduler.current = next;
}

process_t* process_obj_create()
{
    return HH_ADDR(fetch_pool(task_allocator));
}

void schedule_process(process_t* process)
{
    add_link_forward(&tasks, process);   
}

void idle_task_handler()
{
    print_fmt("Idle task run!\n");
    for(;;)
    {
        asm volatile("pause");
    }
}