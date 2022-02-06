#pragma once
#include <stdint.h>

struct str
{
    const char* data;
    uint32_t length; 
};
typedef struct str string_t;

struct str_split
{
    string_t left;
    string_t right;
};
typedef struct str_split string_split_t;

uint32_t strlen(const char* str);
uint8_t str_starts(string_t string, char test);
uint8_t str_ends(string_t string, char test);
uint8_t str_cmp(string_t a, string_t b);
string_t cstr(const char* str);
string_split_t splitstr(string_t string, char delimeter);