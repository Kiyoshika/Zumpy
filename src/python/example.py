from zumpy import array
import itertools

arr = array([3,2], 'int32')
arr.fill(10)

for i in range(arr.shape[0]):
    for j in range(arr.shape[1]):
        print(str(arr[[i,j]]) + " ", end = '')
    print("")

sub_idx = [range(3), range(3), range(3)]

# get all combinations of indices that would be used to subset the new array
# will eventually need to translate this into C and write the Python wrapper
def get_index_combinations(sub_idx):
    bounds = []
    current_idx = []
    for i in range(len(sub_idx)):
        bounds.append(len(sub_idx[i]))
        current_idx.append(0)

    while True:
        print(current_idx)
        current_idx[len(current_idx)-1] += 1
        for i in reversed(range(len(current_idx))):
            if current_idx[i] >= bounds[i]:
                current_idx[i] = 0
                if i == 0:
                    break
                current_idx[i-1] += 1
        if current_idx == [0,0,0]:
            break

get_index_combinations(sub_idx)