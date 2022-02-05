from zumpy import array
from random import randint, seed

arr = array()
arr.create([5,2], 'int32')

# set seed for reproducibility
seed(5021)

# fill array with random values
for i in range(arr.shape[0]):
    for j in range(arr.shape[1]):
        arr[i,j] = randint(0,50)

print("Full Array:")
print(arr)

print("Sum: ", arr.sum())