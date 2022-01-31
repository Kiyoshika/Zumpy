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

int cmp(const void* a, const void* b)
{
    return *(size_t*)a - *(size_t*)b;
}

// iterate over the boolean array for secondary index to check if a row should be kept or not
bool check_row_boolean(filter_type ftype, bool* keep_row_arr, size_t keep_row_arr_size)
{
    printf("\nKeep Row Arr (Size %zu):\n", keep_row_arr_size);
    for (size_t i = 0; i < keep_row_arr_size; ++i)
        printf("%s ", keep_row_arr[i] ? "true" : "false");
    printf("\n");

    switch(ftype)
    {
        case ANY: // any value in secondary index matches
            for (size_t i = 0; i < keep_row_arr_size; ++i)
                if (keep_row_arr[i])
                    return true;
            return false;

        case ALL: // all values in secondary index match
            for (size_t i = 0; i < keep_row_arr_size; ++i)
                if (!keep_row_arr[i])
                    return false;
            return true;
    }
}

void arr_filter(array* arr, bool (*filter)(void*), size_t* secondary_indices, size_t secondary_indices_size, filter_type ftype)
{
    size_t* bounds = malloc(sizeof(size_t) * arr->shape_size);
    size_t* current_idx = malloc(sizeof(size_t) * arr->shape_size);
    size_t total_combinations = 1;
    for (size_t i = 0; i < arr->shape_size; ++i)
    {
        total_combinations *= arr->arr_shape[i];
        bounds[i] = arr->arr_shape[i];
        current_idx[i] = 0; // initially set the index to [0,0,...,0]
    }

    size_t primary_index = current_idx[0];

    // if user doesn't pass secondary indices, use all of them by default
    bool secondary_idx_dynamic = false; // keep track if secondary idx was dynamically allocated
    bool custom_idx = false;
    // for filtering, we check every combination of index besides the 0th (the "primary" index).
    // to be general, a trick to compute this for any dimension is to take the total size and divide it by the
    // dimension of the 0th index.
    size_t keep_row_arr_size;
    if (secondary_indices == NULL)
    {
        secondary_indices = malloc(sizeof(size_t) * arr->arr_shape[arr->shape_size - 1]);
        secondary_indices_size = arr->arr_shape[arr->shape_size - 1];
        for (size_t i = 0; i < secondary_indices_size; ++i)
            secondary_indices[i] = i;
        secondary_idx_dynamic = true;
        keep_row_arr_size = arr->total_size / arr->arr_shape[0];
    }
    else
    {
        // this is mimicking above logic except user passes a custom size for last shape dimension
        // which we must account for
        custom_idx = true;
        keep_row_arr_size = 1;
        for (size_t i = 0; i < arr->shape_size - 1; ++i)
            keep_row_arr_size *= arr->arr_shape[i];
        keep_row_arr_size *= secondary_indices_size;
        keep_row_arr_size /= arr->arr_shape[0];
        // if user passed their own secondary indices, make sure they're sorted to use bsearch
        qsort(secondary_indices, secondary_indices_size, sizeof(size_t), cmp);
    }

    size_t keep_row_iter = 0;

    // used to compute the logical on whether to keep a row. Stores result in row_logical
    bool* keep_row_arr = malloc(sizeof(bool) * keep_row_arr_size);
    for (size_t i = 0; i < keep_row_arr_size; ++i)
        keep_row_arr[i] = false;

    // records true/false on primary index on whether to keep the row
    bool* row_logical = malloc(sizeof(bool) * arr->arr_shape[0]);
    size_t row_logical_iter = 0;
    bool first_pass_through = true;
    while (true)
    {
        // if user passes custom secondary idx they want to search, then only pay
        // attention to those indices
        if (custom_idx)
        {
            int* item = bsearch(&current_idx[arr->shape_size - 1], secondary_indices, secondary_indices_size, sizeof(size_t), cmp);
            if (item == NULL)
            {
                // increment index and continue
                current_idx[arr->shape_size - 1]++;
                goto next_iter;
            }
        }

        if (current_idx[0] == primary_index + 1)
        {
            printf("New Row Arr:\n");
            printf("%s - %s - %s - %s - %s - %s\n\n",
                   keep_row_arr[0] ? "true" : "false",
                   keep_row_arr[1] ? "true" : "false",
                   keep_row_arr[2] ? "true" : "false",
                   keep_row_arr[3] ? "true" : "false",
                   keep_row_arr[4] ? "true" : "false",
                   keep_row_arr[5] ? "true" : "false"
                   );
            row_logical[row_logical_iter] = check_row_boolean(ftype, keep_row_arr, keep_row_arr_size);
            // reset boolean array
            for (size_t i = 0; i < keep_row_arr_size; ++i)
                keep_row_arr[i] = false;
            primary_index++;
            row_logical_iter++;
            keep_row_iter = 0;
        }

        if (filter(arr_at(arr, current_idx)))
            keep_row_arr[keep_row_iter] = true;
        else
            keep_row_arr[keep_row_iter] = false;
        printf("Row Iter: %zu; Index: (%zu, %zu, %zu); Value: %d; Expected: %s; Actual: %s\n", keep_row_iter, current_idx[0], current_idx[1], current_idx[2], *(int32_t*)arr_at(arr, current_idx), filter(arr_at(arr, current_idx)) ? "true" : "false", keep_row_arr[keep_row_iter] ? "true" : "false");

        current_idx[arr->shape_size - 1]++;
        keep_row_iter++;
        // a flag used to jump to when skipping indices if user passes custom secondary idx
        next_iter:
        for (size_t i = arr->shape_size; i-- > 0;)
        {
            if (current_idx[i] >= bounds[i])
            {
                current_idx[i] = 0;
                if (i == 0)
                    break;
                current_idx[i-1]++;
            }
        }

        if (check_index_zero(current_idx, arr->shape_size))
            break;
    }

    // check final row
    row_logical[arr->arr_shape[0] - 1] = check_row_boolean(ftype, keep_row_arr, keep_row_arr_size);
    for (size_t i = 0; i < arr->arr_shape[0]; ++i)
        printf("%s\n", row_logical[i] ? "true" : "false");

    // TODO: Take row_logical and populate a new array (or resize current array) with filtered rows.

    free(bounds);
    free(current_idx);
    free(keep_row_arr);
    free(row_logical);
    if (secondary_idx_dynamic) // free secondary idx if it was dynamically allocated by algorithm
        free(secondary_indices);
}