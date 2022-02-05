from zumpy import array

arr = array()
arr.create([5], 'int32')
arr.fill(10)
arr[2] = 22

print(arr.at(2))

arr2 = array([3,3], 'int32')
arr2.fill(20)

print(arr2.at([0,2]))