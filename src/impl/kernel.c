#include "kernel.h"
#include "devmgr.h"
#include "print.h"
#include "limine.h"

void debug_char_output(uint8_t c) { out_serial(COM1, c); }

void initialize()
{
    init_gdt();
    init_idt();

    read_cpu_features();

    init_serial(COM1);

    init_print(debug_char_output);

    init_pic();
    init_pit();
    init_keyboard();
    init_memory_manager(); 
    init_virtual_memory();

    init_syscall();
    init_scheduler();
    init_devices();
    init_serial_devices();
}

void kernel_start(void)
{
    initialize();

    struct device_meta* dev = query_device("COM1");
    if(dev)
    {
        struct stream_device* sdev = (struct stream_device*)dev->assoc_dev;

        uint8_t item;
        for(;;)
        {
            if(sdev->read(dev, &item, 0, 1))
                print_fmt("{char}", item);
        }
    }

    for(;;)
    {
        asm("hlt");
    }
}