#include "utils.h"
#include "rtc.h"

const char* hexDict = "0123456789ABCDEF";

void itoah(uint64_t value, char* buffer)
{
    uint8_t* ptr = (uint8_t*)&value;
    
    for(int i = 0; i < 16; i += 2)
    {
        buffer[14 - i] = hexDict[(*ptr & 0xF0) >> 4];
        buffer[15 - i] = hexDict[(*ptr & 0x0F)];

        ptr++;
    }

    buffer[16] = '\0';
}