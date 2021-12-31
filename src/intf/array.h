#pragma once
#include <stdint.h>

#define array(T) T*
#define init_array(T, count) initialize_array(sizeof(T), count)
#define push_array(arr, ...) ensure_capacity(&arr); inc_size(arr); (arr)[array_size(arr) - 1] = __VA_ARGS__

struct array_data
{
    uint64_t size;
    uint64_t capacity;
    uint64_t allocated;
    uint64_t element_size;
};
typedef struct array_data array_data_t;

array_data_t* get_array_data(array(void) array);

array(void) initialize_array(uint64_t size, uint64_t capacity);
uint64_t array_capacity(array(void) array);
uint64_t array_size(array(void) array);
void inc_size(array(void) array);

void ensure_capacity(array(void)* array);