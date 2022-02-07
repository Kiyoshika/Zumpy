# Code Organization
This is a README doc to help digest the contents of each implementation file. I tried organizing it somewhat cleanly instead of dumping the entire implementation into one file.
## Contents:
* [access.c](#accessc) ([source code](access.c))
* [filter.c](#filterc) ([source code](filter.c))
* [maths.c](#mathsc) ([source code](maths.c))
* [print.c](#printc) ([source code](print.c))
* [slice.c](#slicec) ([source code](slice.c))
* [zumpy.c](#zumpyc) ([source code](zumpy.c))
* [zumpy_internal.c](#zumpyc) ([source code](zumpy_internal.c))

---

## access.c
This file contains implementations for functions related to accessing the array; either getting or setting values.
### Contains:
* arr_at
* arr_set
* arr_fill

---

## filter.c
This file contains the implementation for the filtering algorithm.
### Contains:
* arr_filter
* arr_multi_filter

---

## maths.c
This file contains implementations for mathematical functions.
### Contains:
* arr_sum

---

## print.c
This file contains the implementation for the print function.
### Contains:
* arr_print

---

## slice.c
This file contains the implementation for the slice algorithm.
### Contains:
* arr_slice

---

## zumpy.c
This file contains the implementations for managing memory.
### Contains:
* arr_init
* arr_free

---

## zumpy_internal.c
This file contains implementations of common functions used internally in different algorithms. These functions aren't exposed in the public API.

---
