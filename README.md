# Zumpy
An array module inspired by numpy written in C using Python's ctypes purely for learning.

The name "Zumpy" is derived from the popular library "numpy" but using the first letter of my name, Z. 

# How to Use
Firstly, I wouldn't use this right now since it's experimental and also barely functional.

However, if you DO want to use it, you can export just the python folder within src and import it just like the example.py. Just be sure to keep the ext folder in the same directory since it's a direct dependency.

# The Internals...
Even though I use the verbage "2D" and "3D" array, internally every array is stored as a one-dimensional void pointer. The multiple dimensions are just mathematical offset calculations to mimick multi-dimensional arrays. Check out the C code if you're interested in how this is done.

# Documenation
I am using Doxygen to generate LaTex/Man/PDF documentation. For the PDF, go to doc/latex directory and open refman.pdf. There are some known formatting errors in the examples and those will be fixed later. Prioritizing the library functionality over formatting issues at the moment.

# Example
This is an example highlighting a couple of the current main features. Creating an array, setting values and filtering. There is also array slicing but it's not shown in this example (yet).
```python
from zumpy import array
from ctypes import *
from random import randint, seed

# set seed for reproducibility
seed(400)

# the underlying C code uses void* pointers, so we have to cast
# to an appropriate type for Python to make sense of it
def myfilter(x):
    x = cast(x, POINTER(c_int32))
    return x.contents.value > 10

# create an empty 3x2 array
arr = array([3,2], 'int32')

# fill array with random values
for i in range(arr.shape[0]):
    for j in range(arr.shape[1]):
        arr[i,j] = randint(0, 50) # set index (i, j) to a random int

# check column index 1 for values greater than 10.
# since we are filtering on one column, ANY/ALL doesn't
# make a difference here
filter_one_column = arr.filter(myfilter, [1], 'ANY')

# passing an empty list will default to check all column indices.
# we also use the "ALL" setting which means that ALL columns
# must meet the condition in order for the row to be returned.
# if ANY is used, then ANY column (at least one column) must
# match the condition to return a row.
filter_two_column = arr.filter(myfilter, [], 'ALL')

print("Original Array:")
print(arr)

print("Filtered Column 1:")
print(filter_one_column)

print("Filtered Both Columns:")
print(filter_two_column)
```

Output:
```
Original Array:
19 35 
16 50 
5 36 

Filtered Column 1:
19 35 
16 50 
5 36 

Filtered Both Columns:
19 35 
16 50 
```
As you can see in the example, the first filter returned the entire array again because we are only checking index 1 > 10 (the "second" column) in which case all results were true. However the second filter only returned the first two rows since the last row contained a value < 10 and we set the setting to "ALL". If we used "ANY" then it would have returned the entire array as 36 > 10.
