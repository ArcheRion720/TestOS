#include "pic.h"

void pic_init()
{
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

    outport8(PIC_MASTER_DATA, 0xFD);
}

void pic_send_eoi(uint8_t irq)
{
    if(irq >= 8)
        outport8(PIC_SLAVE_CMD, PIC_CMD_EOI);
    outport8(PIC_MASTER_CMD, PIC_CMD_EOI);
}