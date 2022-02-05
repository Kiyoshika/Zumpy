#include "include/zumpy.h"
#include "include/zumpy_internal.h"

void* arr_at(array* arr, size_t* index)
{
    if (arr->data)
    {
        int t_shape_size = arr->shape_size;
        return ((char*)(arr->data + arr->type_size*calculate_offset(arr, index, t_shape_size)));
    }

    return NULL;
}

void arr_set(array* arr, size_t* index, void* value)
{
    // only do anything if data is non-empty
    if (arr->data)
    {
        int t_shape_size = arr->shape_size;
        memcpy(((char*)(arr->data + arr->type_size*calculate_offset(arr, index, t_shape_size))), value, arr->type_size);
    }
}

void arr_fill(array* arr, void* value)
{
    // only do anything if data is non-empty
    if (arr->data)
        for (size_t i = 0; i < arr->total_size; ++i)
            memcpy((char*)(arr->data + i*arr->type_size), value, arr->type_size);
}