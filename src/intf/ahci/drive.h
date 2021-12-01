#pragma once
#include <stdint.h>

typedef uint8_t (*drive_function)(struct drive*, uint64_t, uint64_t, uint8_t*);

struct drive 
{
    uint16_t flags;
    uint16_t type;
    uint32_t port;
    char label[32];

    drive_function read;
    drive_function write;
};

typedef struct drive drive_t;

uint8_t get_drive_count();
drive_t* get_drive(uint8_t index);
void register_drive(drive_t drive);