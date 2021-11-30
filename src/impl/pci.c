#include "pci.h"
#include "hal.h"
#include "memory/pmm.h"
#include "utils.h"

static pci_t* pci;
char msg_buffer[17];

uint16_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunction = (uint32_t)function;

    uint32_t addr = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunction << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outport32(PCI_CONFIG_ADDR, addr);
    uint16_t result = (uint16_t)((inport32(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return result;
}

#define PCI_READ(offset) pci_config_read(bus, device, function, offset)

void check_function(uint8_t bus, uint8_t device, uint8_t function)
{
    pci_entry_t* func = &(pci->busses[bus].devices[device].functions[function]);

    func->num.bus = bus;
    func->num.device = device;
    func->num.function = function;

    printf("Registered PCI device at: %ixb:%ixb:%ixb (%ixw : %ixw)\n", bus, device, function, PCI_READ(0), PCI_READ(2));

    func->header.vendor             = PCI_READ(0);
    func->header.device             = PCI_READ(2);
    func->header.command            = PCI_READ(4);
    func->header.status             = PCI_READ(6);
    func->header.revision           = (uint8_t) PCI_READ(8);
    func->header.prog_if            = PCI_READ(8) >> 8;
    func->header.subclass_code      = (uint8_t) PCI_READ(10);
    func->header.class_code         = PCI_READ(10) >> 8;
    func->header.cache_line_size    = (uint8_t) PCI_READ(12);
    func->header.latency_timer      = PCI_READ(12) >> 8;
    func->header.header_type        = PCI_READ(14);
    func->header.bist               = PCI_READ(14) >> 8;
    func->header.bar[0]             = PCI_READ(16) | (PCI_READ(18) << 16);
    func->header.bar[1]             = PCI_READ(20) | (PCI_READ(22) << 16);
    func->header.bar[2]             = PCI_READ(24) | (PCI_READ(26) << 16);
    func->header.bar[3]             = PCI_READ(28) | (PCI_READ(30) << 16);
    func->header.bar[4]             = PCI_READ(32) | (PCI_READ(34) << 16);
    func->header.bar[5]             = PCI_READ(36) | (PCI_READ(38) << 16);
    func->header.cis_pointer        = PCI_READ(40) | (PCI_READ(42) << 16);
    func->header.subsystem_vendor   = PCI_READ(44);
    func->header.subsystem          = PCI_READ(46);
    func->header.expansion_rom      = PCI_READ(48) | (PCI_READ(50) << 16);
    func->header.capabilities       = PCI_READ(52);
    func->header.interrupt_line     = (uint8_t) PCI_READ(60);
    func->header.interrupt_pin      = PCI_READ(60) >> 8;
    func->header.min_grant          = (uint8_t) PCI_READ(62);
    func->header.max_latency        = PCI_READ(62) >> 8;
}

void check_device(uint8_t bus, uint8_t device)
{
    uint8_t function = 0;
    uint16_t vendor = PCI_READ(0);

    if(vendor == 0xFFFF)
        return;

    check_function(bus, device, function);
    uint8_t header = (uint8_t) PCI_READ(14);

    if((header & 0x80) != 0)
    {
        for(function = 1; function < 8; function++)
        {
            vendor = PCI_READ(0);
            if(vendor != 0xFFFF)
                check_function(bus, device, function);
        }
    }
}

void check_bus(uint8_t bus)
{
    for(uint8_t device = 0; device < 32; device++)
    {
        check_device(bus, device);
    }

    printf("Initialised PCI at bus %iu\n", bus);
}

void init_pci()
{
    pci = (pci_t*)alloc_block_adjacent(sizeof(pci_t) / 0x1000);
    check_bus(0);
}

#undef PCI_READ

pci_entry_t* pci_get_function(uint8_t class, uint8_t subclass, uint8_t prog)
{
    for(int bus = 0; bus < 256; bus++)
    {
        for(int device = 0; device < 32; device++)
        {
            for(int function = 0; function < 8; function++)
            {
                pci_entry_t* func = &(pci->busses[bus].devices[device].functions[function]);
                if(func->header.class_code == 0xFF || func->header.class_code == 0)
                    continue;
                
                if(func->header.class_code == class || func->header.class_code == (uint8_t)-1)
                {
                    if(func->header.subclass_code == subclass || func->header.subclass_code == (uint8_t)-1)
                    {
                        if(func->header.prog_if == prog || func->header.prog_if == (uint8_t)-1)
                        {
                            return func;
                        }
                    }
                }
            }
        }
    }

    return 0x0;
}

pci_entry_t* read_entry(uint8_t bus, uint8_t device, uint8_t function)
{
    return &(pci->busses[bus].devices[device].functions[function]);
}