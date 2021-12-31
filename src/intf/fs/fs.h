#pragma once
#include "storage/drive.h"

//TODO: some return type
typedef void (*fs_function)(const char* path, struct file_system*);

struct file_system
{
    partition_t* partition;
    void* metadata;

    fs_function read;
    fs_function write;
};
typedef struct file_system file_system_t;

file_system_t* register_filesystem();
file_system_t* get_filesystem(uint32_t index);
uint32_t get_fs_count();