#include "hal.h"

void irq_unmask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    value = inport8(port) | ~(1 << irq);
    outport8(port, value);
}

void send_eoi(uint8_t irq)
{
    if(irq >= 8)
        outport8(PIC_SLAVE_CMD, PIC_CMD_EOI);
    outport8(PIC_MASTER_CMD, PIC_CMD_EOI);
}

uint8_t inport8(uint16_t portId)
{
    uint8_t result;
    __asm__ ("inb %%dx, %%al": "=a"(result) :"d"(portId));
    return result;
}

uint16_t inport16(uint16_t portId)
{
    uint16_t result;
    __asm__ ("inw %%dx, %%ax": "=a"(result) :"d"(portId));
    return result;
}

uint32_t inport32(uint16_t portId)
{
    //big thanks to qookei for noticing there was uint16_t
    uint32_t result;
    __asm__ ("inl %%dx, %%eax": "=a"(result) : "d"(portId));
    return result;
}

void outport8(uint16_t portId, uint8_t value)
{
    __asm__ ("outb %%al, %%dx" :: "d"(portId), "a"(value));
}

void outport16(uint16_t portId, uint16_t value)
{
    __asm__ ("outw %%ax, %%dx" :: "d"(portId), "a"(value));
}

void outport32(uint16_t portId, uint32_t value)
{
    __asm__ ("outl %%eax, %%dx" :: "d"(portId), "a"(value));
}

uint8_t init_serial(uint16_t com)
{
    outport8(COM_INTERRUPT(com),    0x00);
    outport8(COM_LINE_CONTROL(com), COM_DLAB_BIT);
    outport8(COM_BAUD_LSB(com),     0x0C);
    outport8(COM_BAUD_MSB(com),     0x00);
    outport8(COM_LINE_CONTROL(com), 0x03);
    outport8(COM_FIFO(com),         0xC7);
    outport8(COM_MODEM(com),        0x0B);
    outport8(COM_MODEM(com),        0x1E);
    outport8(com, 0xAE);

    if(inport8(com) != 0xAE)
        return 1;

    outport8(COM_MODEM(com), 0x0F);
    return 0;    
}

uint8_t serial_received(uint16_t com)
{
    return inport8(COM_LINE_STATUS(com)) & COM_LINE_READY;
}

uint8_t in_serial(uint16_t com)
{
    while(serial_received(com) == 0)
        asm("pause");

    return inport8(com);
}

uint8_t is_transmit_empty(uint16_t com)
{
    return inport8(COM_LINE_STATUS(com)) & COM_LINE_FREE;
}

void out_serial(uint16_t com, uint8_t value)
{
    while(is_transmit_empty(com) == 0)
        asm("pause");

    outport8(com, value);
}

void out_serial_str(uint16_t com, const char* str, uint32_t length)
{
    for(uint32_t i = 0; i < length; i++)
    {
        out_serial(com, str[i]);
    }
}

void io_wait(void)
{
    outport8(0x80, 0);
}

uintptr_t read_cr3()
{
    uint64_t result;
    asm ("mov %%cr3, %0" : "=r"(result));
    return result;
}

void mask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    value = inport8(irq) | (1 << irq);
    outport8(port, value);
}

void clear_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    value = inport8(irq) & ~(1 << irq);
    outport8(port, value);
}