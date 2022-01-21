#include "include/zumpy.h"

int main()
{
    array myarr;

    size_t shape[] = {3, 3, 3};
    arr_init(&myarr, shape, 3, INT32);

    int32_t val = 10;
    arr_fill(&myarr, &val);

    // specify the index to access within the "3D" array
    size_t index[] = { 2, 1, 1 };
    printf("%d\n", *(int32_t*)arr_at(&myarr, index));

    arr_free(&myarr);
    return 0;
}