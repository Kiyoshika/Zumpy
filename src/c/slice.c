#include "include/zumpy.h"
#include "include/zumpy_internal.h"

void arr_slice(array* srcarray, size_t** sub_arr_idx, size_t* sub_arr_dims, size_t sub_arr_dims_len, array* subarray)
{
    size_t** indices = get_index_combinations(sub_arr_dims, sub_arr_dims_len, NULL);

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