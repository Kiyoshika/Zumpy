#include "include/zumpy.h"

int main()
{
    array myarr;

    size_t shape[] = {3, 3};
    arr_init(&myarr, shape, 2, INT32);

    // fill array with values of 10
    int32_t val = 10;
    arr_fill(&myarr, &val);

    printf("%f\n", arr_sum(&myarr)); // 90.0

    arr_free(&myarr);
    return 0;
}