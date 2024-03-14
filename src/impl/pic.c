#include "x86/pic.h"
#include "x86/hal.h"
#include "print.h"

void init_pic()
{
    __asm__("cli");
    outport8(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outport8(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outport8(PIC_MASTER_DATA, 0x20);
    io_wait();
    outport8(PIC_SLAVE_DATA, 0x28);
    io_wait();
    outport8(PIC_MASTER_DATA, 4);
    io_wait();
    outport8(PIC_SLAVE_DATA, 2);
    io_wait();
    outport8(PIC_MASTER_DATA, ICW4_8086);
    io_wait();
    outport8(PIC_SLAVE_DATA, ICW4_8086);
    io_wait();

    outport8(PIC_MASTER_DATA, 0x1);
    outport8(PIC_SLAVE_DATA, 0x0);
    
    print_fmt("Initialised PIC\n");
    __asm__("sti");
}