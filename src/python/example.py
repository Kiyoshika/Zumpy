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