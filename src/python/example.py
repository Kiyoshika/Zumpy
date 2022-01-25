from zumpy import array

# create a 3x2 array filled with 10s
arr = array([3,2], 'int32')
arr.fill(10)

# take all row indices from the 0th column
subarray = arr.slice([range(arr.shape[0]), [0]])

print("Original Array:")
for i in range(arr.shape[0]):
    for j in range(arr.shape[1]):
        print(str(arr[[i,j]]) + " ", end = '')
    print("")
print("")

print("Sliced Array (First Column):")
for i in range(subarray.shape[0]):
    for j in range(subarray.shape[1]):
        print(str(subarray[[i,j]]) + " ", end = '')
    print("")