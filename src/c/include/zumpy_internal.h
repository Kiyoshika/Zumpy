/*
 * This header file stores common internal functions that are used in implementations
 */

#ifndef ZUMPY_ZUMPY_INTERNAL_H
#define ZUMPY_ZUMPY_INTERNAL_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// offset calculation which dynamically scales with N-dimensions
size_t calculate_offset(array* arr, size_t* index, int shape_size);

// internal function to check if an index [0,0,...,0] equals zero
// i.e, if each element is zero.
bool check_index_zero(size_t* index, size_t len);

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
size_t** get_index_combinations(size_t* sub_arr_dims, size_t sub_arr_dims_len, size_t** indices);

int cmp(const void* a, const void* b);


#endif //ZUMPY_ZUMPY_INTERNAL_H
