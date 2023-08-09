#include <stdint.h>
#include "memory/pmm.h"
#include "ringbuf.h"
#include "devmgr.h"
#include "hal.h"
#include "interrupts.h"
#include "print.h"

#define SERIAL_DRV_ID           0x100
#define SERIAL_DRV_CTRL_BAUD    0x1
#define SERIAL_DRV_CTRL_INTR    0x2

struct serial_device_data
{
    uint16_t irq;
    uint16_t port;
    uint16_t transmitter_state;
    struct ringbuffer_meta* in_buffer;
    struct ringbuffer_meta* out_buffer;
    struct stream_device device;
};

static struct serial_device_data serial_dev_data[4];

static inline serial_can_read(uint16_t com_port)
{
    return inport8(COM_LINE_STATUS(com_port)) & COM_LINE_READY;
}

static inline serial_can_write(uint16_t com_port)
{
    return inport8(COM_LINE_STATUS(com_port)) & COM_LINE_FREE; 
}

static inline serial_transmitter_empty(uint16_t com_port)
{
    return inport8(COM_LINE_STATUS(com_port)) & COM_LINE_BUFEMPT;
}

uint8_t serial_port_probe(uint16_t com)
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
        return 0;

    outport8(COM_MODEM(com), 0x0F);
    return 1;   
}

void serial_enable_interrupts(uint16_t com)
{
    outport8(COM_INTERRUPT(com), 1);
}

uintmax_t serial_write(struct device_meta* dev, uint8_t* ubuffer, uintmax_t offset, uintmax_t size)
{
    if(dev->owner != SERIAL_DRV_ID && dev->device_type != DEV_STREAM)
        __builtin_trap();

    struct stream_device* sdev = (struct stream_device*)dev->assoc_dev;
    struct serial_device_data* data = (struct serial_device_data*)sdev->private_data;

    uintmax_t written = 0;
    written = ringbuffer_write_buffer(data->out_buffer, ubuffer, size);

    if(written < size)
    {
        serial_flush(dev);
        return written;
    }

    uint8_t c;
    if(written > 0 && (c = ringbuffer_read(data->out_buffer)))
    {
        outport8(data->port, c);
    }

    return written;
}

uintmax_t serial_read(struct device_meta* dev, uint8_t* ubuffer, uintmax_t offset, uintmax_t size)
{
    if(dev->owner != SERIAL_DRV_ID && dev->device_type != DEV_STREAM)
        __builtin_trap();
        // return 0;

    struct stream_device* sdev = (struct stream_device*)dev->assoc_dev;
    struct serial_device_data* data = (struct serial_device_data*)sdev->private_data;

    return ringbuffer_read_buffer(data->in_buffer, ubuffer, size);
}

void serial_interrupt(registers_t* regs)
{
    for(uint64_t i = 0; i < 4; i++)
    {
        if(serial_dev_data[i].irq != (regs->interrupt - 32))
            continue;

        while(
            serial_can_read(serial_dev_data[i].port) && 
            ringbuffer_write(serial_dev_data[i].in_buffer, inport8(serial_dev_data[i].port))
        );

        uint8_t buffer = 0;
        while(serial_can_write(serial_dev_data[i].port))
        {
            if(!ringbuffer_read_buffer(serial_dev_data[i].out_buffer, &buffer, 1))
                break;

            outport8(serial_dev_data[i].port, buffer);
        }
    }
}

void serial_flush(struct device_meta* dev)
{
    if(dev->owner != SERIAL_DRV_ID && dev->device_type != DEV_STREAM)
        __builtin_trap();

    struct stream_device* sdev = (struct stream_device*)dev->assoc_dev;
    struct serial_device_data* data = (struct serial_device_data*)sdev->private_data;

    data->in_buffer->last_read = data->in_buffer->last_write;
}

void serial_control(struct device_meta* dev, uint64_t ctrl, void* arg)
{
    __builtin_trap();
}

void init_serial_devices()
{
    const uint16_t com_ports[] = {0x3F8, 0x2F8, 0x3E8, 0x2E8};
    const uint8_t* com_names[] = {"COM1", "COM2", "COM3", "COM4"};

    struct
    {
        uint8_t irq3_enable;
        uint8_t irq4_enable;
    } irqs = {0, 0};

    for(uint32_t i = 0; i < 4; i++)
    {
        if(serial_port_probe(com_ports[i]))
        {
            struct device_meta* com_dev = device_register(com_names[i]);
            com_dev->assoc_dev = &serial_dev_data[i].device;
            com_dev->device_type = DEV_STREAM;

            struct stream_device* com_stream = (struct stream_device*)com_dev->assoc_dev;

            com_dev->owner = SERIAL_DRV_ID;
            com_stream->private_data = &serial_dev_data[i];

            if(i % 2 == 0)
            {
                serial_dev_data[i].irq = 4;
                irqs.irq4_enable = 1;
            }
            else
            {
                serial_dev_data[i].irq = 3;
                irqs.irq3_enable = 1;
            }

            serial_dev_data[i].port = com_ports[i];

            struct ringbuffer_meta* rb_in = ringbuffer_create(PMM_PAGE_SIZE);
            if(!rb_in)
            {
                stream_device_dispose(com_dev);
                continue;
            }
            serial_dev_data[i].in_buffer = rb_in;

            struct ringbuffer_meta* rb_out = ringbuffer_create(PMM_PAGE_SIZE);
            if(!rb_out)
            {
                stream_device_dispose(com_dev);
                ringbuffer_destroy(rb_in);
                continue;
            }
            serial_dev_data[i].out_buffer = rb_out;
            serial_dev_data[i].transmitter_state = serial_transmitter_empty(com_ports[i]);

            com_stream->read  = &serial_read;
            com_stream->write = &serial_write;
            com_stream->flush = &serial_flush;
            serial_enable_interrupts(com_ports[i]);
        }
    }

    if(irqs.irq3_enable)
        register_isr_handler(35, (isr_t)&serial_interrupt);

    if(irqs.irq4_enable)
        register_isr_handler(36, (isr_t)&serial_interrupt);
}