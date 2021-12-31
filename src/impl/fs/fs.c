#include "fs/fs.h"
#include "memory/pmm.h"
#include <stdint.h>

file_system_t filesystems[32];
uint32_t fs_count;

file_system_t* register_filesystem()
{
    return &filesystems[fs_count++];
}

uint32_t get_fs_count()
{
    return fs_count;
}

file_system_t* get_filesystem(uint32_t index)
{
    return &filesystems[index];
}