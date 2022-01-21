#include "include/zumpy.h"

// offset calculation which dynamically scales with N-dimensions
size_t calculate_offset(array* arr, size_t* index, int shape_size)
{
    if (shape_size == 1) return *index;
    else
        return index[shape_size - 1] + arr->arr_shape[shape_size - 1] * calculate_offset(arr, index, shape_size - 1);
}

// internal function for getting the size of the data type based off the enum
int get_type_size(type dtype)
{
    switch (dtype)
    {
        case INT32:
            return sizeof(int32_t);
            break;
        case FLOAT:
            return sizeof(float);
            break;
    }

    return -1;
}

void arr_init(array* arr, size_t* arr_shape, size_t shape_size, type dtype)
{
    arr->type_size = get_type_size(dtype);
    arr->arr_shape = malloc(sizeof(size_t) * shape_size);
    for (size_t i = 0; i < shape_size; ++i)
        arr->arr_shape[i] = arr_shape[i];
    arr->shape_size = shape_size;
    arr->dtype = dtype;
    size_t alloc_size = arr_shape[0];
    if (shape_size > 1)
        for (size_t i = 1; i < shape_size; ++i)
            alloc_size *= arr_shape[i];
    arr->total_size = alloc_size;

    void *alloc = malloc(sizeof(arr->type_size) * alloc_size);
    if (alloc)
        arr->data = alloc;
}

void arr_free(array* arr)
{
    if (arr->data)
    {
        free(arr->data);
        free(arr->arr_shape);
        arr->data = NULL;
        arr->arr_shape = NULL;
    }
}

void* arr_at(array* arr, size_t* index)
{
    if (arr->data)
    {
        int t_shape_size = arr->shape_size;
        return ((char*)(arr->data + arr->type_size*calculate_offset(arr, index, t_shape_size)));
    }

    return NULL;
}

void arr_fill(array* arr, void* value)
{
    // only do anything if data is non-empty
    if (arr->data)
    {
        for (size_t i = 0; i < arr->total_size; ++i)
            memcpy((char*)(arr->data + i*arr->type_size), value, arr->type_size);
    }
}
