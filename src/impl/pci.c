#include "pci.h"
#include "hal.h"
#include "memory_mgmt.h"
#include "utils.h"
#include "print.h"
#include "devmgr.h"

static struct pool_allocator* pci_header_allocator;

static inline uint32_t pci_data_address(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset)
{
    return (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1l << 31);
}

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    outport32(PCI_CONFIG_ADDR, pci_data_address(bus, device, function, offset));
    return inport32(PCI_CONFIG_DATA);
}

struct pci_header pci_read_common_header(uint8_t bus, uint8_t device, uint8_t function)
{
    union 
    {
        uint32_t words[4];
        struct pci_header header;
    } 
    data;

    data.words[0] = pci_config_read(bus, device, function, 0x0);
    data.words[1] = pci_config_read(bus, device, function, 0x4);
    data.words[2] = pci_config_read(bus, device, function, 0x8);
    data.words[3] = pci_config_read(bus, device, function, 0xC);

    return data.header;
}

struct pci_header pci_read_header(uint8_t bus, uint8_t device, uint8_t function)
{
    union 
    {
        uint32_t words[72];
        struct pci_header header;
    } 
    data;

    data.header = pci_read_common_header(bus, device, function);

    uint32_t limit = ((PCI_HEADER_TYPE(data.header)) == 0x2) ? 0x44 : 0x3C;
    uint32_t index = 4;
    for(uint8_t offset = 0x10; offset <= limit; offset += 0x4)
    {
        data.words[index++] = pci_config_read(bus, device, function, offset);
    }

    return data.header;
}

void pci_scan_function(uint8_t bus, uint8_t device, uint8_t func);

void pci_scan_bus(uint8_t bus)
{
    for(uint8_t dev = 0; dev < 32; dev++)
        pci_scan_device(bus, dev);
}

void pci_scan_device(uint8_t bus, uint8_t device)
{
    struct pci_header header = pci_read_common_header(bus, device, 0);
    if(header.vendor == PCI_INVALID)
        return;

    pci_scan_function(bus, device, 0);
    if(PCI_IS_MULTI_FUNC(header))
    {
        for(uint8_t func = 1; func < 8; func++)
        {
            header = pci_read_common_header(bus, device, func);
            if(header.vendor != PCI_INVALID)
                pci_scan_function(bus, device, func);
        }
    }
}

void pci_scan_function(uint8_t bus, uint8_t device, uint8_t func)
{
    struct pci_header header = pci_read_header(bus, device, func);

    switch(PCI_HEADER_TYPE(header))
    {
        case PCI_HEADER_PCI_BRIDGE:
            print_fmt("Found bridge at {xbyte}:{xbyte}:{xbyte}\n", &bus, &device, &func);
            print_fmt("\t{xbyte}-{xbyte}-{xbyte}\n", &header.type1.primary_bus, &header.type1.secondary_bus, &header.type1.subordinate_bus);
            if((header.class == 0x6) && (header.subclass == 0x4))
            {
                pci_scan_bus(header.type1.secondary_bus);
            }
            return;

        case PCI_HEADER_DEV:
        {
            // print_fmt("Registered device at {xbyte}:{xbyte}:{xbyte}\n", &bus, &device, &func);
            // print_fmt("\t{xbyte} | {xbyte} | {xbyte}\n", &header.class, &header.subclass, &header.progif);
            struct device_meta* meta = device_register("PCI device");
            meta->device_type = DEV_PCI;
            meta->assoc_dev = HH_ADDR(pool_fetch(pci_header_allocator));
            *((struct pci_header*)meta->assoc_dev) = header;
            return;
        }
        case PCI_HEADER_CARDBUS_BRIDGE:
            __builtin_trap();
            return;
    }
}

void init_pci()
{
    pci_header_allocator = pool_allocator_acquire(sizeof(struct pci_header), PMM_PAGE_SIZE);
    struct pci_header header = pci_read_common_header(0, 0, 0);

    if(PCI_IS_MULTI_FUNC(header))
    {
        for(uint8_t func = 0; func < 8; func++)
        {
            if(pci_read_header(0, 0, func).vendor == PCI_INVALID)
                continue;

            pci_scan_bus(func);
        }
    }
    else
    {
        pci_scan_bus(0);
    }
}