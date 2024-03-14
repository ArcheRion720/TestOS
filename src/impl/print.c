#include <stdarg.h>
#include "print.h"
#include "x86/hal.h"

#define HEX_FORMAT_8  0
#define HEX_FORMAT_16 1
#define HEX_FORMAT_32 2
#define HEX_FORMAT_64 3

static uint32_t formats_count = 0;
static struct print_format_data formats[64];
static print_char_func_ptr print_char_function;

void register_print_format_impl(string_t identifier, print_callback_t callback)
{
    formats[formats_count].identifier = identifier;
    formats[formats_count].callback = callback;
    formats_count++;
}

void handle_print_format(string_t identifier, void* item)
{
    for(uint8_t i = 0; i < formats_count; i++)
    {
        if(str_cmp(identifier, formats[i].identifier))
        {
            formats[i].callback(item);
        }
    }
}

void print_fmt_impl(string_t fmt, va_list args, void(*put_char)(uint8_t))
{
    do
    {
        string_split_t split = str_split(fmt, '{');

        for(uint32_t i = 0; i < split.left.length; i++)
            put_char(split.left.data[i]);

        if(split.right.length > 0)
        {
            // identifier | rest of text
            split = str_split(split.right, '}');
            handle_print_format(split.left, va_arg(args, void*));
        }

        fmt = split.right;
    }
    while(fmt.length > 0);
}

void print_fmt(const char* format, ...)
{
    string_t format_str = cstr(format);

    va_list args;
    va_start(args, format);
    print_fmt_impl(format_str, args, print_char_function);
    va_end(args);
}

//
// Default formats implementations
//

void print_uns(uintmax_t value)
{
    static uint8_t buffer[20];

    uint8_t buf = 0;
    do
    {
        buffer[buf] = '0' + (value % 10);
        value /= 10;
        buf++;
    }
    while(value);

    for(uint8_t i = buf; i --> 0;)
    {
        print_char_function(buffer[i]);
    }
}

void print_char(uint8_t value)
{
    print_char_function(value);
}

const uint8_t* digits_dict = (const uint8_t*)"0123456789ABCDEF";

void print_hex(uint8_t hex_size, uintmax_t value)
{
    //hex_size :    0 -> 8  bit
    //              1 -> 16 bit
    //              2 -> 32 bit
    //              3 -> 64 bit

    print_char_function('0');
    print_char_function('x');

    static uint16_t buffer[8];
    for(uint32_t i = (uint32_t)(1 << hex_size); i --> 0;)
    {
        buffer[i] = (digits_dict[(value & 0xF0) >> 4]) + (digits_dict[value & 0x0F] << 8);
        value >>= 8;
    }

    for(uint32_t i = 0; i < (uint32_t)(1 << (hex_size + 1)); i++)
    {
        print_char_function(((uint8_t*)buffer)[i]);
    }
}

void print_u8(uint8_t* num)   { print_uns((uintmax_t)*num); }
void print_u16(uint16_t* num) { print_uns((uintmax_t)*num); }
void print_u32(uint32_t* num) { print_uns((uintmax_t)*num); }
void print_u64(uint64_t* num) { print_uns((uintmax_t)*num); }

void print_hex8(uint8_t* num)   { print_hex(HEX_FORMAT_8,  (uintmax_t)*num); }
void print_hex16(uint16_t* num) { print_hex(HEX_FORMAT_16, (uintmax_t)*num); }
void print_hex32(uint32_t* num) { print_hex(HEX_FORMAT_32, (uintmax_t)*num); }
void print_hex64(uint64_t* num) { print_hex(HEX_FORMAT_64, (uintmax_t)*num); }

void init_print(print_char_func_ptr func)
{
    print_char_function = func;

    register_print_format("byte", print_u8);
    register_print_format("short", print_u16);
    register_print_format("int", print_u32);
    register_print_format("long", print_u64);

    register_print_format("xbyte", print_hex8);
    register_print_format("xshort", print_hex16);
    register_print_format("xint", print_hex32);
    register_print_format("xlong", print_hex64);

    register_print_format("char", print_char);
}