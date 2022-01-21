# Zumpy
An array module inspired by numpy written in C using Python's ctypes purely for learning.

The name "Zumpy" is derived from the popular library "numpy" but using the first letter of my name, Z. 

# How to Use
Firstly, I wouldn't use this right now since it's experimental and also barely functional.

However, if you DO want to use it, you can export just the python folder within src and import it just like the example.py. Just be sure to keep the ext folder in the same directory since it's a direct dependency.

# The Internals...
Even though I use the verbage "2D" and "3D" array, internally every array is stored as a one-dimensional void pointer. The multiple dimensions are just mathematical offset calculations to mimick multi-dimensional arrays. Check out the C code if you're interested in how this is done.

# Example
This is the dummy example of the first working build. This will create a 1D, 2D and 3D array and fill it with int32 values of 10.
```python
from zumpy import array

# create a 1D array with 5 elements
arr1D = array([5], 'int32')
arr1D.fill(10)

print("1D Array:\n=================")
for i in range(arr1D.shape[0]):
    print(str(arr1D.at([i])) + " ", end='')
print("\n")



# create a 2x3 array for a total of 6 elements
arr2D = array([2,3], 'int32')
arr2D.fill(10)

print("2D Array:\n=================")
for i in range(arr2D.shape[0]):
    for j in range(arr2D.shape[1]):
        print(str(arr2D.at([i,j])) + " ", end='')
    print("")



# create a 3x3x3 array for a total of 27 elements
arr3D = array([3,3,3], 'int32')
arr3D.fill(10)

print("\n3D Array:\n=================")
for i in range(arr3D.shape[0]):
    for j in range(arr3D.shape[1]):
        for k in range(arr3D.shape[2]):
            print(str(arr3D.at([i,j,k])) + " ", end='')
        print("")
    print("")
```
Output:
```
1D Array:
=================
10 10 10 10 10 

2D Array:
=================
10 10 10 
10 10 10 

3D Array:
=================
10 10 10 
10 10 10 
10 10 10 

10 10 10 
10 10 10 
10 10 10 

10 10 10 
10 10 10 
10 10 10 
```
