#pragma once

#include <stdint.h>
#include <stddef.h>
#include "hal.h"
#include "memory/pmm.h"

struct pci_num
{
    uint8_t bus;
    uint8_t device;
    uint8_t function;
};
typedef struct pci_num pci_num_t;

struct pci_header
{
    uint16_t device;
    uint16_t vendor;
	uint16_t status;
    uint16_t command;
    uint8_t class_code;
	uint8_t subclass_code;
	uint8_t prog_if;
	uint8_t revision;
	uint8_t bist;
	uint8_t header_type;
	uint8_t latency_timer;
	uint8_t cache_line_size;
    uint32_t bar[6];
	uint32_t cis_pointer;
	uint16_t subsystem;
	uint16_t subsystem_vendor;
	uint32_t expansion_rom;
	uint8_t reserved1;
    uint8_t capabilities;
	uint32_t reserved3;
	uint8_t max_latency;
	uint8_t min_grant;
	uint8_t interrupt_pin;
	uint8_t interrupt_line;
};
typedef struct pci_header pci_header_t;

struct pci_entry
{
    pci_num_t num;
	pci_header_t header;
};
typedef struct pci_entry pci_entry_t;

struct pci_device
{
    pci_entry_t functions[8];
};
typedef struct pci_device pci_device_t;

struct pci_bus
{
    pci_device_t devices[32];
};
typedef struct pci_bus pci_bus_t;

struct pci
{
    pci_bus_t busses[256];
};
typedef struct pci pci_t;

void init_pci();
uint16_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
pci_entry_t* pci_get_function(uint8_t class, uint8_t subclass, uint8_t prog);
pci_entry_t* read_entry(uint8_t bus, uint8_t device, uint8_t function);