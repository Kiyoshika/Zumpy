from zumpy import array

arr2D = array([2,3], 'float')
arr2D.fill(10.0)
arr2D[[0,2]] = 20.0

print("2D Array:\n=================")
for i in range(arr2D.shape[0]):
    for j in range(arr2D.shape[1]):
        print(str(arr2D[[i,j]]) + " ", end = '')
    print("")

print("\nSum: ", arr2D.sum())