#include "include/zumpy.h"
#include "include/zumpy_internal.h"

// internal function to print the contents of an arbitrary-dimensional array.
// the implementation is extremely similar to get_index_combinations but still a bit different
void print(array* arr, size_t* sub_arr_dims, size_t sub_arr_dims_len)
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

    while (true)
    {
        switch (arr->dtype)
        {
            case INT32:
                printf("%d ", *(int32_t*)arr_at(arr, current_idx));
                break;
            case FLOAT:
                printf("%f ", *(float*)arr_at(arr, current_idx));
                break;
        }

        current_idx[sub_arr_dims_len - 1]++;
        for (size_t i = sub_arr_dims_len; i-- > 0;)
        {
            if (current_idx[i] >= bounds[i])
            {
                current_idx[i] = 0;
                if (i == 0)
                    break;
                current_idx[i-1]++;
                printf("\n");
            }
        }

        if (check_index_zero(current_idx, sub_arr_dims_len))
            break;
    }

    free(bounds);
    free(current_idx);
}

// a wrapper on print() to expose to the C API
void arr_print(array* arr)
{
    print(arr, arr->arr_shape, arr->shape_size);
}