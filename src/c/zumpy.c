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
        for (size_t i = 0; i < arr->total_size; ++i)
            memcpy((char*)(arr->data + i*arr->type_size), value, arr->type_size);
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

float arr_sum(array* arr)
{
    float sum = 0.0;
    switch (arr->dtype)
    {
        case INT32:
            for (size_t i = 0; i < arr->total_size; ++i)
                sum += *(int32_t*)((char*)(arr->data + arr->type_size*i));
            return sum;
            break;
        case FLOAT:
            for (size_t i = 0; i < arr->total_size; ++i)
                sum += *(float*)((char*)(arr->data + arr->type_size*i));
            return sum;
            break;
    }
    return sum; // 0.0
}

// internal function to check if an index [0,0,...,0] equals zero
// i.e, if each element is zero.
bool check_index_zero(size_t* index, size_t len)
{
    size_t sum = 0;
    for (size_t i = 0; i < len; ++i)
        sum += index[i];
    return sum == 0;
}

// internal function to get all index combinations to be used for array slicing.

// sub_arr_idx : first index level indicates dimension, second index level indicates indices desired for
// that dimension. E.g: sub_arr_idx = [ [1,2,3], [2] ]. dimension 0 we take indices [1,2,3] and
// dimension 1 we take index 2.

// sub_arr_dims : each index corresponds to the length of the inner array within sub_arr_idx. From
// the example above it would be [ 3, 1 ] since [1,2,3] is length 3 and [2] is length 1.

// sub_arr_dims_len : length of the array sub_arr_dims. From example above, it would be 2.

// indices : the collection of all possible indices based off the combinations of sub_arr_idx. This is
// the parameter we store our results into to use when slicing arrays. Pass this as NULL as we will
// allocate the memory inside this function.
size_t** get_index_combinations(size_t** sub_arr_idx, size_t* sub_arr_dims, size_t sub_arr_dims_len, size_t** indices)
{
    size_t* bounds = malloc(sizeof(size_t) * sub_arr_dims_len);
    size_t* current_idx = malloc(sizeof(size_t) * sub_arr_dims_len);
    size_t total_combinations = 1;
    for (size_t i = 0; i < sub_arr_dims_len; ++i)
    {
        total_combinations *= sub_arr_dims[i];
        bounds[i] = sub_arr_dims[i];
        current_idx[i] = 0; // initially set the index to [0,0,...,0]
    }

    indices = malloc(sizeof(size_t*) * total_combinations);
    for (size_t i = 0; i < total_combinations; ++i)
        indices[i] = malloc(sizeof(size_t) * sub_arr_dims_len);

    size_t current_iter = 0;

    while (true)
    {
        // store current index into our indices array
        for (size_t i = 0; i < sub_arr_dims_len; ++i)
            indices[current_iter][i] = current_idx[i];
        current_iter++;


        current_idx[sub_arr_dims_len - 1]++;
        for (size_t i = sub_arr_dims_len; i-- > 0;)
        {
            if (current_idx[i] >= bounds[i])
            {
                current_idx[i] = 0;
                if (i == 0)
                    break;
                current_idx[i-1]++;
            }
        }

        if (check_index_zero(current_idx, sub_arr_dims_len))
            break;
    }

    free(bounds);
    free(current_idx);

    return indices;
}

void arr_slice(array* srcarray, size_t** sub_arr_idx, size_t* sub_arr_dims, size_t sub_arr_dims_len, array* subarray)
{
    size_t** indices = get_index_combinations(sub_arr_idx, sub_arr_dims, sub_arr_dims_len, indices);

    size_t total_combinations = 1;
    for (size_t i = 0; i < sub_arr_dims_len; ++i)
        total_combinations *= sub_arr_dims[i];

    // size up array
    arr_init(subarray, sub_arr_dims, sub_arr_dims_len, srcarray->dtype);

    size_t index_set[sub_arr_dims_len];
    size_t index_get[sub_arr_dims_len];

    for (size_t i = 0; i < total_combinations; ++i)
    {
        for (size_t j = 0; j < sub_arr_dims_len; ++j)
            index_set[j] = indices[i][j];

        for (size_t j = 0; j < sub_arr_dims_len; ++j)
            index_get[j] = sub_arr_idx[j][indices[i][j]];

        arr_set(subarray, index_set, arr_at(srcarray, index_get));
    }

    for (size_t i = 0; i < total_combinations; ++i)
        free(indices[i]);
    free(indices);

}