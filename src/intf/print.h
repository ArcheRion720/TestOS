#pragma once
#include <stdint.h>
#include "datastruct/string.h"

#define register_print_format(fmt, callback) register_print_format_impl(MAKE_STRING(fmt), (void(*)(void*))callback)

typedef void print_callback_t(void*);
typedef void (*print_char_func_ptr)(uint8_t);

struct print_format_data
{
    string_t identifier;
    print_callback_t* callback;
};

void register_print_format_impl(string_t, print_callback_t);
void print_fmt(const char*, ...);
void print_char(uint8_t);

void init_print(print_char_func_ptr);