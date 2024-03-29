#include "datastruct/string.h"
#include <stdint.h>

uint32_t strlen(const uint8_t* str)
{
    uint32_t result = 0;
    while(str[result++] != '\0')
        continue;

    return result;
}

uint8_t str_starts(string_t string, uint8_t test)
{
    if(string.length == 0)
        return 0;
    return string.data[0] == test;
}

uint8_t str_ends(string_t string, uint8_t test)
{
    if(string.length == 0)
        return 0;
    return string.data[string.length] == test;
}

string_t cstr(const uint8_t* str)
{
    return (string_t){ str, strlen(str) };
}

string_split_t str_split(string_t string, uint8_t delimeter)
{
    uint32_t index;
    for(index = 0; index < string.length; index++)
    {
        if(string.data[index] == delimeter)
        {
            return (string_split_t)
            {
                (string_t) { string.data, index },
                (string_t) { string.data + index + 1, string.length - index - 1}
            };
        }
    }

    return (string_split_t) { string, 0 };
}

uint8_t str_cmp(string_t a, string_t b)
{
    if(a.length != b.length)
        return 0;

    for(uint32_t i = 0; i < a.length; i++)
        if(a.data[i] != b.data[i])
            return 0;
            
    return 1;
}