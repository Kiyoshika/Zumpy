from zumpy import array

arr1D = array([5], 'int32')
arr1D.fill(10)

print("1D Array:\n=================")
for i in range(arr1D.shape[0]):
    print(str(arr1D.at([i])) + " ", end='')
print("\n")



arr2D = array([2,3], 'int32')
arr2D.fill(10)

print("2D Array:\n=================")
for i in range(arr2D.shape[0]):
    for j in range(arr2D.shape[1]):
        print(str(arr2D.at([i,j])) + " ", end='')
    print("")



arr3D = array([3,3,3], 'int32')
arr3D.fill(10)

print("\n3D Array:\n=================")
for i in range(arr3D.shape[0]):
    for j in range(arr3D.shape[1]):
        for k in range(arr3D.shape[2]):
            print(str(arr3D.at([i,j,k])) + " ", end='')
        print("")
    print("")