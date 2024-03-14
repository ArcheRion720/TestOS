#include "kernel.h"
#include "print.h"
#include "boot_request.h"

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
    // init_keyboard();
    init_memory_manager();
    
    init_dev_object_manager();

    init_syscall();
    init_scheduler();

    init_pci();
}

void dev_iterate(struct splay_tree_node* node)
{
    struct dev_object* obj = container_of(node, struct dev_object, node);
    print_fmt("Device: {long} {int}\n", &obj->id, &obj->int_count);

    struct dev_object_interface* intf;
    linked_list_foreach(intf, &obj->interfaces, dev_link)
    {
        if(intf->type == DEV_INTERFACE_IDENTIFY_PCI)
        {
            struct pci_header* pci = (struct pci_header*)(&intf->spec_data);
            print_fmt("\tInterface: {xlong} {long} {xbyte}:{xbyte}:{xbyte}\n", 
                &intf->type, 
                &intf->length,
                &pci->class,
                &pci->subclass,
                &pci->progif);
        }
    }

    if(node->left)
    {
        dev_iterate(node->left);
    }

    if(node->right)
    {
        dev_iterate(node->right);
    }
}

void kernel_start(void)
{
    initialize();
    dev_iterate(dev_object_manager.root);

    for(;;)
    {
        asm("hlt");
    }
}
