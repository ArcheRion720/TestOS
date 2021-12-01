#include "ahci/drive.h"

drive_t drives[32];
uint8_t drive_count; 

drive_t* get_drive(uint8_t index)
{
    if(index >= drive_count)
        return 0;

    return &drives[index];
}

uint8_t get_drive_count()
{
    return drive_count;
}

void register_drive(drive_t drive)
{
    drives[drive_count++] = drive;
}