#pragma once
#include <stdint.h>
#include <stddef.h>

#define PCI_INVALID     0xFFFF

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_HEADER_DEV              0x0
#define PCI_HEADER_PCI_BRIDGE       0x1
#define PCI_HEADER_CARDBUS_BRIDGE   0x2

struct pci_header
{
    uint16_t vendor;
    uint16_t device;
    uint16_t command;
    uint16_t status;
    uint8_t revision;
    uint8_t progif;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;

    union
    {
        struct
        {
            uint32_t bar[6];
            uint32_t cis;
            uint16_t subvendor;
            uint16_t subsystem;
            uint32_t rom_base;
            uint8_t capabilities;
            uint8_t reserved[7];
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint8_t min_grant;
            uint8_t max_latency;
        } __attribute__((packed))
        type0;
        
        struct
        {
            uint32_t bar[2];
            uint8_t primary_bus;
            uint8_t secondary_bus;
            uint8_t subordinate_bus;
            uint8_t secondary_latency_timer;
            uint8_t io_base;
            uint8_t io_limit;
            uint16_t secondary_status;
            uint16_t memory_base;
            uint16_t memory_limit;
            uint16_t prefetch_memory_base;
            uint16_t prefetch_memory_limit;
            uint32_t prefetch_base_high;
            uint32_t prefetch_limit_high;
            uint16_t io_base_high;
            uint16_t io_limit_high;
            uint8_t capabilities;
            uint8_t reserved[3];
            uint32_t expansion_rom_base;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint16_t bridge_control;
        } __attribute__((packed))
        type1;

        struct 
        {
            uint32_t base_address;
            uint8_t capabilities_list_offset;
            uint8_t reserved0;
            uint16_t secondary_status;
            uint8_t pci_bus;
            uint8_t cardbus_bus;
            uint8_t subordinate_bus;
            uint8_t cardbus_latency_timer;
            uint32_t memory_base_address0;
            uint32_t memory_limit_address0;
            uint32_t memory_base_address1;
            uint32_t memory_limit_address1;
            uint32_t io_base_address0;
            uint32_t io_limit0;
            uint32_t io_base_address1;
            uint32_t io_limit1;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint16_t bridge_control;
            uint16_t subsystem_device;
            uint16_t subsystem_vendor;
            uint32_t legacy_address;
        } __attribute__((packed))
        type2;
    };
} __attribute__((packed));

struct pci_device
{
    struct pci_header header;
};

#define PCI_HEADER_TYPE(header) (header.header_type & 0x7F)
#define PCI_IS_MULTI_FUNC(header) (header.header_type & 0x80)

void init_pci();