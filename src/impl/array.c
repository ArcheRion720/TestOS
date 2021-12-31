#include "array.h"
#include "memory/pmm.h"
#include "utils.h"

array(void) initialize_array(uint64_t size, uint64_t capacity)
{
    malloc_report_t rep = malloc_ex((size * capacity) + sizeof(array_data_t));
    array_data_t* data = rep.data;

    data->capacity = capacity;
    data->element_size = size;
    data->allocated = rep.size;
    data->size = 0;
    
    return rep.data + sizeof(array_data_t);
}

array_data_t* get_array_data(array(void) array)
{
    return (array_data_t*)(array - sizeof(array_data_t));
}

uint64_t array_capacity(array(void) array)
{
    return get_array_data(array)->capacity;
}

uint64_t array_size(array(void) array)
{
    return get_array_data(array)->size;
}

void inc_size(array(void) array)
{
    array_data_t* data = get_array_data(array);
    data->size = data->size + 1;
}

void ensure_capacity(array(void)* array)
{
    array_data_t* data = get_array_data(*array);
    if(data->capacity > data->size)
        return;

    array(void) old = *array;
    *array = initialize_array(data->element_size, data->capacity * 2);

    array_data_t* new_data = get_array_data(*array);
    new_data->size = data->size;

    memcpy(old, *array, data->element_size * data->size);
    free(data, data->allocated);
}