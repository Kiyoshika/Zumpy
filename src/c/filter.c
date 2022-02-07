#include "include/zumpy.h"
#include "include/zumpy_internal.h"

// iterate over the boolean array for secondary index to check if a row should be kept or not
bool check_row_boolean(filter_type ftype, bool* keep_row_arr, size_t keep_row_arr_size)
{
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

void arr_filter(array* arr, bool (*filter)(void*), size_t* secondary_indices, size_t secondary_indices_size, filter_type ftype, array* dest)
{
    size_t* bounds = malloc(sizeof(size_t) * arr->shape_size);
    size_t* current_idx = malloc(sizeof(size_t) * arr->shape_size);

    for (size_t i = 0; i < arr->shape_size; ++i)
    {
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

    // check how many rows we're keeping to size up new array
    size_t kept_rows = 0;
    for (size_t i = 0; i < arr->arr_shape[0]; ++i)
        if (row_logical[i])
            kept_rows++;

    size_t* set_index = malloc(sizeof(size_t) * arr->shape_size);
    for (size_t i = 0; i < arr->shape_size; ++i)
        set_index[i] = 0;

    size_t new_shape[arr->shape_size];


    if (kept_rows > 0)
    {
        // free up array if it's not empty already
        if (dest->data != NULL)
            arr_free(dest);

        size_t new_shape[arr->shape_size];
        new_shape[0] = kept_rows;
        for (size_t i = 1; i < arr->shape_size; ++i)
            new_shape[i] = arr->arr_shape[i];

        arr_init(dest, new_shape, arr->shape_size, arr->dtype);
        // re-iterate over indices and only populate the indices that match the kept primary indices
        while (true)
        {
            // if we are keeping this row (checked against primary index)
            if (row_logical[current_idx[0]])
            {
                arr_set(dest, set_index, arr_at(arr, current_idx));
                set_index[dest->shape_size - 1]++;
                for (size_t i = dest->shape_size; i-- > 0;)
                {
                    if (set_index[i] >= bounds[i])
                    {
                        set_index[i] = 0;
                        if (i == 0)
                            break;
                        set_index[i-1]++;
                    }
                }
            }

            current_idx[arr->shape_size - 1]++;
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

    }
    else
    {
        // if no rows match, make "empty" array with zero shape
        if (dest->data != NULL)
            arr_free(dest);
        size_t new_shape[arr->shape_size];
        for (size_t i = 0; i < arr->shape_size; ++i)
            new_shape[i] = 0;

        arr_init(dest, new_shape, arr->shape_size, arr->dtype);
    }

    free(bounds);
    free(current_idx);
    free(set_index);
    free(keep_row_arr);
    free(row_logical);
    if (secondary_idx_dynamic) // free secondary idx if it was dynamically allocated by algorithm
        free(secondary_indices);

}

void arr_multi_filter(array*arr, bool (**filters)(void*), size_t** secondary_indices, size_t* secondary_indices_sizes, size_t n_filters, filter_type* ftypes, array* dest)
{
    for (size_t i = 0; i < n_filters; ++i)
        arr_filter(arr, filters[i], secondary_indices[i], secondary_indices_sizes[i], ftypes[i], dest);

}