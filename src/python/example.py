from zumpy import array

arr = array([3,2], 'int32')
arr.fill(10)

for i in range(arr.shape[0]):
    for j in range(arr.shape[1]):
        print(str(arr[[i,j]]) + " ", end = '')
    print("")