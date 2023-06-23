#pragma once
#include <stdint.h>
#include "utils.h"

#define MAKE_STRING(x) (string_t){(uint8_t*)(x), sizeof(x) - 1}
#define str_loop(it, str) \
    uint32_t MACRO_VAR(str_length) = str.length; \
    for(uint8_t const* it = str.data; MACRO_VAR(str_length) > 0; MACRO_VAR(str_length)--, it++)

struct str
{
    const uint8_t* data;
    uint32_t length; 
};
typedef struct str string_t;

struct str_split
{
    string_t left;
    string_t right;
};
typedef struct str_split string_split_t;

uint32_t strlen(const uint8_t* str);
uint8_t str_starts(string_t string, uint8_t test);
uint8_t str_ends(string_t string, uint8_t test);
uint8_t str_cmp(string_t a, string_t b);
string_t cstr(const uint8_t* str);
string_split_t str_split(string_t string, uint8_t delimeter);