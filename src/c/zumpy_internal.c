#include "include/zumpy.h"
#include "include/zumpy_internal.h"

// offset calculation which dynamically scales with N-dimensions
size_t calculate_offset(array* arr, size_t* index, int shape_size)
{
    if (shape_size == 1) return *index;
    else
        return index[shape_size - 1] + arr->arr_shape[shape_size - 1] * calculate_offset(arr, index, shape_size - 1);
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
size_t** get_index_combinations(size_t* sub_arr_dims, size_t sub_arr_dims_len, size_t** indices)
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

int cmp(const void* a, const void* b)
{
    return *(size_t*)a - *(size_t*)b;
}