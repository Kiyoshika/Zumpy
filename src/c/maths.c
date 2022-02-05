#include "include/zumpy.h"

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