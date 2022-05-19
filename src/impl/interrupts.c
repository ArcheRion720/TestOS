#include "interrupts.h"
#include "hal.h"
#include "utils.h"
#include "tasking.h"

idtr_descriptor_t IDT[256];
isr_t handlers[256];

void init_intdt()
{
    idtr_t ptr = (idtr_t)
    {
        limit: (uint16_t)(256 * 8 - 1),
        base: (uint64_t)IDT,
    };

    __asm__("lidt %0" :: "m"(ptr));

    register_intdt(0 , (uint64_t)&isr0 );
    register_intdt(1 , (uint64_t)&isr1 );
    register_intdt(2 , (uint64_t)&isr2 );
    register_intdt(3 , (uint64_t)&isr3 );
    register_intdt(4 , (uint64_t)&isr4 );
    register_intdt(5 , (uint64_t)&isr5 );
    register_intdt(6 , (uint64_t)&isr6 );
    register_intdt(7 , (uint64_t)&isr7 );
    register_intdt(8 , (uint64_t)&isr8 );
    register_intdt(9 , (uint64_t)&isr9 );
    register_intdt(10, (uint64_t)&isr10);
    register_intdt(11, (uint64_t)&isr11);
    register_intdt(12, (uint64_t)&isr12);
    register_intdt(13, (uint64_t)&isr13);
    register_intdt(14, (uint64_t)&isr14);
    register_intdt(15, (uint64_t)&isr15);
    register_intdt(16, (uint64_t)&isr16);
    register_intdt(17, (uint64_t)&isr17);
    register_intdt(18, (uint64_t)&isr18);
    register_intdt(19, (uint64_t)&isr19);
    register_intdt(20, (uint64_t)&isr20);
    register_intdt(21, (uint64_t)&isr21);
    register_intdt(22, (uint64_t)&isr22);
    register_intdt(23, (uint64_t)&isr23);
    register_intdt(24, (uint64_t)&isr24);
    register_intdt(25, (uint64_t)&isr25);
    register_intdt(26, (uint64_t)&isr26);
    register_intdt(27, (uint64_t)&isr27);
    register_intdt(28, (uint64_t)&isr28);
    register_intdt(29, (uint64_t)&isr29);
    register_intdt(30, (uint64_t)&isr30);
    register_intdt(31, (uint64_t)&isr31);

    register_intdt(32, (uint64_t)&irq0);
    register_intdt(33, (uint64_t)&irq1);
    register_intdt(34, (uint64_t)&irq2);
    register_intdt(35, (uint64_t)&irq3);
    register_intdt(36, (uint64_t)&irq4);
    register_intdt(37, (uint64_t)&irq5);
    register_intdt(38, (uint64_t)&irq6);
    register_intdt(39, (uint64_t)&irq7);
    register_intdt(40, (uint64_t)&irq8);
    register_intdt(41, (uint64_t)&irq9);
    register_intdt(42, (uint64_t)&irq10);
    register_intdt(43, (uint64_t)&irq11);
    register_intdt(44, (uint64_t)&irq12);
    register_intdt(45, (uint64_t)&irq13);
    register_intdt(46, (uint64_t)&irq14);
    register_intdt(47, (uint64_t)&irq15);

    register_intdt(69, (uint64_t)&irq69);

    __asm__("sti");

    log("Initialised interrupts");
}

void register_intdt(uint32_t code, uint64_t addr)
{
    IDT[code].offset015 = (addr & 0xffff);
    IDT[code].selector = 0x28;
    IDT[code].zero = 0;
    IDT[code].type = 0x8E;
    IDT[code].offset1631 = (addr >> 16) & 0xFFFF;
    IDT[code].offset3263 = (addr >> 32);
    IDT[code].zerohigh = 0;
}

#define PRINT_REG(x) printf_ll(#x ": %ixq\n", regs.x);

void isr_handler(registers_t regs)
{
    //printf("Interrupt => [%iu]\n", regs.interrupt);
    send_eoi(regs.interrupt - 32);
    if(regs.interrupt < 32)
    {
        //panic!
        printf_ll("Error occured [%iu]\n", regs.interrupt);
        printf_ll("Error code: [%iu]\n", regs.error);
        printf_ll("rip: %ixq\n", regs.rip);
        PRINT_REG(rax);
        PRINT_REG(rbx);
        PRINT_REG(rcx);
        PRINT_REG(rdx);
        PRINT_REG(rdi);
        PRINT_REG(rsi);
        PRINT_REG(rbp);
        printf_ll("\n");
        PRINT_REG(cr2);
        PRINT_REG(cr3);
        PRINT_REG(cs);
        PRINT_REG(ds);
        PRINT_REG(es);
        PRINT_REG(fs);
        PRINT_REG(gs);

        for(uint32_t i = 0; i < 2; i++)
        {
            task_t* tsk = get_task(i);
            printf_ll("Process %iu CS: [%ixq]\n", tsk->id, tsk->regs.cs);
        }

        __asm__ ("hlt");
    }    
    if(regs.interrupt >= 32 && handlers[regs.interrupt] != 0)
    {
        isr_t handler = handlers[regs.interrupt];
        handler(&regs);
    }
}

void register_isr_handler(uint32_t code, isr_t handler)
{
    handlers[code] = handler;
}