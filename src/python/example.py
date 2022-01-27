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