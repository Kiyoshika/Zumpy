#ifndef ZUMPY_ZUMPY_H
#define ZUMPY_ZUMPY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @param arr
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



float arr_sum_row(array* arr, size_t row_index);



float arr_sum_column(array* arr, size_t col_index);

#endif //ZUMPY_ZUMPY_H
