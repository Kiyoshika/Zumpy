# Zumpy
An array module inspired by numpy written in C using Python's ctypes purely for learning.

The name "Zumpy" is derived from the popular library "numpy" but using the first letter of my name, Z. 

# How to Use
Firstly, I wouldn't use this right now since it's experimental and also barely functional.

However, if you DO want to use it, you can export just the python folder within src and import it just like the example.py. Just be sure to keep the ext folder in the same directory since it's a direct dependency.

# The Internals...
Even though I use the verbage "2D" and "3D" array, internally every array is stored as a one-dimensional void pointer. The multiple dimensions are just mathematical offset calculations to mimick multi-dimensional arrays. Check out the C code if you're interested in how this is done.

# Example
This is the dummy example of the first working build. This will create a 3x2 array filled with 10s and slice the first column into a separate 3x1 array.
```python
from zumpy import array

# create a 3x2 array filled with 10s
arr = array([3,2], 'int32')
arr.fill(10)

# take all row indices from the 0th column
subarray = arr.slice([range(arr.shape[0]), [0]])

print("Original Array:")
print(arr)

print("Subarray:")
print(subarray)
```

Output:
```
Original Array:
10 10 
10 10 
10 10 

Subarray:
10 
10 
10 
```
