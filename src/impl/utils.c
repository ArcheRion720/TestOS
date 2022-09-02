#include "utils.h"
#include "rtc.h"
#include "terminal.h"
#include <stdarg.h>

const uint8_t* digits_dict = (const uint8_t*)"0123456789ABCDEF";

void str_cpy(uint8_t* src, uint8_t* dest, uint32_t count)
{
    for(uint32_t i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }
}

void memset(uint8_t* addr, uint8_t value, uint64_t count)
{
    for(uint64_t i = 0; i < count; i++)
    {
        addr[i] = value;
    }
}

void memcpy(uint8_t* src, uint8_t* dest, uint64_t count)
{
    while(count--)
    {
        *dest++ = *src++;
    }
}

#define FORMAT_TIME(index, var)     \
    temp = var / 10;                \
    buffer[index] = '0' + temp;     \
    buffer[index + 1] = '0' + (var - 10 * temp);

void write_time(uint8_t* buffer, int* ptr, time_t time)
{
    uint8_t temp;
    FORMAT_TIME(0, time.day_month);
    buffer[2] = '.';
    FORMAT_TIME(3, time.month);
    buffer[5] = '.';
    buffer[6] = '2';
    buffer[7] = '0';
    FORMAT_TIME(8, time.year);
    buffer[10] = ' ';
    FORMAT_TIME(11, time.hour);
    buffer[13] = ':';
    FORMAT_TIME(14, time.minute);
    buffer[16] = ':';
    FORMAT_TIME(17, time.second);

    *ptr += 19;
}

#undef FORMAT_TIME

void write_int_hex(uint8_t* buffer, int* ptr, uint8_t hex_size, uint64_t value)
{
    //hex_size :    0 -> 8  bit
    //              1 -> 16 bit
    //              2 -> 32 bit
    //              3 -> 64 bit

    uint16_t* buf = (uint16_t*)buffer;
    buf += (1 << hex_size) - 1;
    for(int i = 0; i < 1 << hex_size; i++)
    {
        *(buf--) = (digits_dict[(value & 0xF0) >> 4]) + (digits_dict[value & 0x0F] << 8);
        value >>= 8;
        *ptr += 2;
    }
}

void write_int_dec(uint8_t* buffer, int* ptr, uint64_t value)
{
    uint64_t digits = value;
    uint8_t i = 0;
    do
    {
        i++;
        buffer++;
        digits /= 10;
    }
    while(digits);

    *ptr += i;

    do
    {
        *--buffer = digits_dict[value % 10];
        value /= 10;
    }
    while(value);
}

static uint8_t buffer[256];

int vprintf(const char* format, va_list list)
{
    int result = 0;
    char* ptr = 0;
    uint32_t length;

    for(int i = 0; format[i]; i++)
    {
        if(format[i] != '%')
        {
            buffer[result++] = format[i];
            continue;
        }

        i++;
        switch(format[i])
        {
            case 'i':
                i++;
                switch(format[i])
                {
                    case 'x':
                        i++;
                        switch(format[i])
                        {
                            case 'b':
                                write_int_hex(&buffer[result], &result, 0, va_arg(list, uint64_t));
                                break;
                            case 'w':
                                write_int_hex(&buffer[result], &result, 1, va_arg(list, uint64_t));
                                break;
                            case 'd':
                                write_int_hex(&buffer[result], &result, 2, va_arg(list, uint64_t));
                                break;
                            case 'q':
                                write_int_hex(&buffer[result], &result, 3, va_arg(list, uint64_t));
                                break;
                        }
                        break;
                    case 'u':
                        write_int_dec(&buffer[result], &result, va_arg(list, uint64_t));
                        break;
                }
                break;

            case 't':
                write_time(&buffer[result], &result, va_arg(list, time_t));
                break;
            case 's':
                switch(format[i + 1])
                {
                    case '.':
                        i++;
                        ptr = va_arg(list, char*);
                        length = va_arg(list, uint32_t);
                        for(uint32_t i = 0; i < length; i++)
                        {
                            buffer[result++] = *ptr;
                            ptr++;
                        }
                        break;
                
                    default:
                        ptr = va_arg(list, char*);
                        while(*ptr)
                        {
                            buffer[result++] = *ptr;
                            ptr++;
                        }
                        break;
                }
                break;
            default:
                return 0;
        }
    }

    terminal_write(buffer, result);
    return result;
}

int printf_ll(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    int i = vprintf(format, list);
    va_end(list);
    return i;
}

int printf(const char* format, ...)
{
    // ticket_lock_acquire(PRINT_LOCK);
    va_list list;
    va_start(list, format);
    int i = vprintf(format, list);
    va_end(list);
    // ticket_lock_release(PRINT_LOCK);
    return i;
}

int log(const char* format, ...)
{
    // ticket_lock_acquire(PRINT_LOCK);
    printf_ll("\x1b[32;1m[INFO]\x1b[0m\t");
    va_list list;
    va_start(list, format);
    int i = vprintf(format, list);
    va_end(list);

    terminal_write("\n", 1);
    // ticket_lock_release(PRINT_LOCK);
    return i;
}

int warn(const char* format, ...)
{
    // ticket_lock_acquire(PRINT_LOCK);
    printf_ll("\x1b[33;1m[WARN]\x1b[0m\t");
    va_list list;
    va_start(list, format);
    int i = vprintf(format, list);
    va_end(list);

    terminal_write("\n", 1);
    // ticket_lock_release(PRINT_LOCK);
    return i;
}
