#pragma once
#include "storage/drive.h"
#include "string.h"

//TODO: some return type
typedef void (*fs_function)(string_t path, struct file_system*);

struct file_system
{
    partition_t* partition;
    void* metadata;

    fs_function read;
    fs_function write;
    fs_function list;
};
typedef struct file_system file_system_t;

file_system_t* register_filesystem();
file_system_t* get_filesystem(uint32_t index);
uint32_t get_fs_count();