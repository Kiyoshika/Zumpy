from zumpy import array

# convert a 3x4 python list into a zumpy array
x = [
        [1,2,3,4],
        [5,6,7,8],
        [9,10,11,12]
]
arr = array()
arr.to_array(x) # by default the dtype is 'int32' so no need to specify here

print("Shape: ", arr.shape)
print("Array:")
print(arr)