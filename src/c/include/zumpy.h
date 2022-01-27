#ifndef ZUMPY_ZUMPY_H
#define ZUMPY_ZUMPY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum { INT32, FLOAT } type;

typedef struct
{
    void *data;
    size_t *arr_shape;
    size_t shape_size;
    size_t type_size;
    size_t total_size;
    type dtype;
} array;

/**
 * Initialize an empty array of arbitrary shape.
 * @param arr Reference (pointer) to an array struct.
 * @param arr_shape A size_t array (decayed to a pointer) indicating the dimensions of the array.
 * @param shape_size The length of the shape; i.e, the total number of dimensions.
 * @param dtype Data type of the array; must be one of INT32 or FLOAT.
 * @code
 * array myarr;
 *
 * size_t shape[] = {3, 3, 3}; // create a 3x3x3 array
 * arr_init(&myarr, shape, 3, INT32); // allocates memory
 *
 * printf("%d %d %d\n", myarr.shape[0], myarr.shape[1], myarr.shape[2]);
 *
 * arr_free(&myarr); // free memory allocated by the array
 * @endcode
 */
void arr_init(array* arr, size_t* arr_shape, size_t shape_size, type dtype);



/**
 * Free up allocated memory taken by the array.
 * @param arr Reference (pointer) to an array struct.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * array myarr;
 *
 * size_t shape[] = {3, 3, 3}; // create a 3x3x3 array
 * arr_init(&myarr, shape, 3, INT32); // allocates memory
 *
 * printf("%d %d %d\n", myarr.shape[0], myarr.shape[1], myarr.shape[2]);
 *
 * arr_free(&myarr); // free memory allocated by the array
 * @endcode
 */
void arr_free(array* arr);



/**
 * Access an element of the array by index.
 * @param arr Reference (pointer) to an array struct.
 * @param index A size_t array (decayed to pointer) indicating the index to access.
 * @code
 * array myarr;
 *
 * size_t shape[] = {3, 3, 3};
 * arr_init(&myarr, shape, 3, INT32);
 *
 * int32_t val = 10;
 * arr_fill(&myarr, &val);
 *
 * // specify the index to access within the "3D" array
 * size_t index[] = { 2, 1, 1 };
 * printf("%d\n", *(int32_t*)arr_at(&myarr, index)); // 10
 *
 * arr_free(&myarr);
 * @endcode
 */
void* arr_at(array* arr, size_t* index);



/**
 * Set a single value within the array.
 * @param arr Reference (pointer) to an array struct.
 * @param index Index to set the value at.
 * @param value Value to set.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * array myarr;
 *
 * size_t shape[] = {3, 3, 3};
 * arr_init(&myarr, shape, 3, INT32);
 *
 * // fill array with values of 10
 * int32_t val = 10;
 * arr_fill(&myarr, &val);
 *
 * // set index 2,1,1 to 20
 * size_t index[] = { 2, 1, 1 };
 * val = 20;
 * arr_set(&myarr, index, &val);
 *
 * printf("%d\n", *(int32_t*)arr_at(&myarr, index)); // 20
 *
 * arr_free(&myarr);
 * @endcode
 */
void arr_set(array* arr, size_t* index, void* value);



/**
 * Fill an array with a constant value.
 * @param arr Reference (pointer) to an array struct.
 * @param value Value to fill the array with.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * array myarr;
 *
 * size_t shape[] = {3, 3, 3};
 * arr_init(&myarr, shape, 3, INT32);
 *
 * // fill "3D" array with values of 10
 * int32_t val = 10;
 * arr_fill(&myarr, &val);
 *
 * // print the contents of the "3D" array
 * size_t index[] = { 0, 0, 0 };
 * for (size_t i = 0; i < shape[0]; ++i)
 * {
 *     index[0] = i;
 *     for (size_t j = 0; j < shape[1]; ++j)
 *     {
 *         index[1] = j;
 *         for (size_t k = 0; k < shape[2]; ++k)
 *         {
 *             index[2] = k;
 *             printf("%d ", *(int32_t*)arr_at(&myarr, index));
 *         }
 *         printf("\n");
 *     }
 *     printf("\n");
 * }
 *
 * arr_free(&myarr);
 * @endcode
 * **Output:**
 * @code
 * 10 10 10
 * 10 10 10
 * 10 10 10
 *
 * 10 10 10
 * 10 10 10
 * 10 10 10
 *
 * 10 10 10
 * 10 10 10
 * 10 10 10
 * @endcode
 */
void arr_fill(array* arr, void* value);



/**
 * Sum all elements in an array.
 * @note For multi-dimensional arrays this will sum ALL cells. If you want to sum a specific row or column, check arr_sum_row(array*) and arr_sum_column(array*).
 * @see arr_sum_row(array*)
 * @see arr_sum_column(array*)
 * @param arr Reference (pointer) to an array struct.
 * @return The sum of all cells as a float.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * array myarr;
 *
 * size_t shape[] = {3, 3};
 * arr_init(&myarr, shape, 2, INT32);
 *
 * // fill array with values of 10
 * int32_t val = 10;
 * arr_fill(&myarr, &val);
 *
 * printf("%f\n", arr_sum(&myarr)); // 90.0
 *
 * arr_free(&myarr);
 * @endcode
 */
float arr_sum(array* arr);



/**
 * Slice an array by specifying a jagged array indicating what indices to pull from which dimensions of a source array and store them into a target aray.
 * @note For the sub array, you DO NOT need to initalize it as it will be initialized in the function for you. But you still must free it. See the example below for a full example.
 * @param srcarray Source array to slice from.
 * @param sub_arr_idx A jagged array indicating the indices to pull from each dimension of srcarray. Index 0 will be an array of indices to extract from dimension 0 of the array and so on for higher indices.
 * @param sub_arr_dims An array indicating the shape of the slice. E.g {3, 1} if your slice will produce a 3x1 array.
 * @param sub_arr_dims_len A value indicating total dimensions that are being sliced.
 * @param subarray Target array to store slices into.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * // set up dimensions to slice
 * // this will take index 0-2 on dimension 0 and index 0 on dimension 1 (slicing one column)
 * size_t dims[2] = {3, 1}; // slice 3x1 array
 * size_t dim0[3] = {0, 1, 2}; // pull index 0-2 from dimension 0 (i.e all rows)
 * size_t dim1[1] = {0}; // pull index 0 from dimension 1 (i.e the first column)
 * size_t* sub_arr_index[2] = { dim0, dim1 };
 *
 * // create example array: 3x3 filled with 10s
 * size_t shape[2] = {3, 3};
 * array arr, sub; // NOTE: DO NOT initialize sub here; it will be initialized for you
 * arr_init(&arr, shape, 2, INT32);
 *
 * int32_t val = 10;
 * arr_fill(&arr, &val);
 *
 * // slice the first column
 * arr_slice(&arr, sub_arr_index, dims, 2, &sub);
 *
 * // print our sliced array (TODO: implement a generic method to do this; this is messy!!)
 * size_t idx[2] = {0,0};
 * for (size_t i = 0; i < sub.arr_shape[0]; ++i) {
 *    idx[0] = i;
 *    for (size_t j = 0; j < sub.arr_shape[1]; ++j) {
 *         idx[1] = j;
 *         printf("%d ", *(int32_t *)arr_at(&sub, idx));
 *     }
 *     printf("\n");
 * }
 *
 * arr_free(&arr);
 * arr_free(&sub);
 * @endcode
 * **Output:**
 * This slices a 3x1 array of 10s from the original 3x3 array (the first column)
 * @code
 * 10
 * 10
 * 10
 * @endcode
 */
void arr_slice(array* srcarray, size_t** sub_arr_idx, size_t* sub_arr_dims, size_t sub_arr_dims_len, array* subarray);



/**
 * Print the contents of an array to the console.
 * @param arr Reference (pointer) to an array struct.
 * @code
 * #include "zumpy.h"
 *
 * // ... other code
 *
 * // initialize 3x2 array
 * size_t shape[] = {3, 2};
 * array arr;
 * arr_init(&arr, shape, 2, INT32);
 *
 * // fill all cells with 10
 * int32_t val = 10;
 * arr_fill(&arr, &val);
 *
 * // print array contents to the console
 * arr_print(&arr);
 *
 * // deallocate
 * arr_free(&arr);
 * @endcode
 */
void arr_print(array* arr);
#endif //ZUMPY_ZUMPY_H
