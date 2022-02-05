# python binding for libZumpy.so
from ctypes import *
import faulthandler

# load library
_libZumpy = CDLL('./ext/libZumpy.so')

# wrapper class
class array_wrapper(Structure):
    _fields_ = [
        ("data", c_void_p),
        ("arr_shape", POINTER(c_size_t)),
        ("shape_size", c_size_t),
        ("type_size", c_size_t),
        ("total_size", c_size_t),
        ("type", c_uint)
    ]

# function prototypes
_libZumpy.arr_init.argtypes = [POINTER(array_wrapper), POINTER(c_size_t), c_size_t, c_size_t]
_libZumpy.arr_init.restype = None

_libZumpy.arr_free.argtypes = [POINTER(array_wrapper)]
_libZumpy.arr_free.restype = None

_libZumpy.arr_at.argtypes = [POINTER(array_wrapper), POINTER(c_size_t)]
_libZumpy.arr_at.restype = c_void_p

_libZumpy.arr_set.argtypes = [POINTER(array_wrapper), POINTER(c_size_t), c_void_p]
_libZumpy.arr_set.restype = None

_libZumpy.arr_fill.argtypes = [POINTER(array_wrapper), c_void_p]
_libZumpy.arr_fill.restype = None

_libZumpy.arr_sum.argtypes = [POINTER(array_wrapper)]
_libZumpy.arr_sum.restype = c_float

_libZumpy.arr_slice.argtypes = [POINTER(array_wrapper), POINTER(POINTER(c_size_t)), POINTER(c_size_t), c_size_t, POINTER(array_wrapper)]
_libZumpy.arr_slice.restype = None

_libZumpy.arr_print.argtypes = [POINTER(array_wrapper)]
_libZumpy.arr_slice.restype = None

_libZumpy.arr_filter.argtypes = [POINTER(array_wrapper), CFUNCTYPE(c_bool, c_void_p), POINTER(c_size_t), c_size_t, c_uint, POINTER(array_wrapper)]
_libZumpy.arr_filter.restype = None

## Array Module
# A simple array class that handles arbitrary dimensions for integer and float types.
class array():
    def _get_type_enum(self, dtype):
        if dtype == 'int32':
            return 0
        elif dtype == 'float':
            return 1

    arr = None
    dtype = None
    shape = None

    ## Create/Initialize an empty array with specified size/dimension and data type.
    # @param shape A list specifying the shape/dimension, e.g [3, 2] for a 3x2 array.
    # @param dtype A string specifying the data type of the array. One of ('int32', 'float'). By default, it's 'int32'.
    #
    # Example:
    #
    # @code
    # from zumpy import array
    #
    # # create 3x2 array of 32-bit integers
    # myarray = array()
    # myarray.create([3,2], 'int32')
    # @endcode
    def create(self, shape, dtype = 'int32'):

        self.arr = array_wrapper()
        self.dtype = dtype
        self.shape = shape

        arr_ptr = pointer(self.arr)

        shape_size = len(self.shape)
        shape_arr = (c_size_t * len(self.shape))(*self.shape)

        type_enum = self._get_type_enum(self.dtype)

        _libZumpy.arr_init(arr_ptr, shape_arr, shape_size, type_enum)

    ## Constructor for array class. Calls create(self, shape, dtype) method.
    # @param shape A list specifying the shape/dimension, e.g [3, 2] for a 3x2 array.
    # @param dtype A string specifying the data type of the array. One of ('int32', 'float'). By default, it's 'int32'.
    #
    # Example:
    #
    # @code
    # from zumpy import array
    #
    # # create 3x2 array of 32-bit integers
    # myarray = array([3,2], 'int32')
    # @endcode
    def __init__(self, shape = None, dtype = 'int32'):
        if shape != None:
            self.create(shape, dtype)

    ## Destructor to deallocate memory from the array. This probably won't ever need to be manually called by the user.
    # This should handle the memory management behind the scenes interacting with the C code to avoid memory leaks.
    def __del__(self):
        arr_ptr = pointer(self.arr)
        _libZumpy.arr_free(arr_ptr)

    ## Override print() call to print the contents of an array.
    # Calls custom print() function implemented in C to output contents in the console.
    #
    # Example:
    #
    # @code
    # print(myarray)
    # @endcode
    def __str__(self):
        arr_ptr = pointer(self.arr)
        _libZumpy.arr_print(arr_ptr)
        return ""

    ## Override print() call to print the contents of an array.
    # Calls custom print() function implemented in C to output contents in the console.
    #
    # Example:
    #
    # @code
    # print(myarray)
    # @endcode
    def __repr__(self):
        self.__str__()

    ## Access an element by index.
    # @param idx A list (or integer for 1D) specifying the index. E.g [1, 2] will access the element at the second row and third column (zero-indexed).
    # @return Returns the value at the specified index.
    #
    # Example:
    #
    # @code
    # myarray.at(2) # access third element in 1D array
    # # note that higher dimensions require list syntax as below:
    # myarray.at([1,4]) # access (1,4)th element in 2D array
    # @endcode
    def at(self, idx):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx

        idx_arr = (c_size_t * len(temp_idx))(*temp_idx)
        # dereference different types
        if self.dtype == 'int32':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_int32)).contents.value
        elif self.dtype == 'float':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_float)).contents.value

        return None

    ## Access an element by index.
    # This is a wrapper around the zumpy.array.at(self, idx) method to use convenient square bracket syntax.
    # @param idx A list specifying the index. E.g [1, 2] will access the element at the second row and third column (zero-indexed).
    # @return Returns the value at the specified index.
    #
    # Example:
    #
    # @code
    # myarray[3]     # access the fourth element in a 1D array
    # myarray[1,2]   # access the (1,2)th element in a 2D array
    # myarray[2,1,1] # so on and so forth...I think you get the idea
    # @endcode
    def __getitem__(self, idx):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx
        return self.at(temp_idx)

    ## Set an element by index
    # @param idx A list (or integer for 1D) specifying the index to set the value at. E.g [1, 2] will set a value at the second row, third column.
    # @param value Value to set at the specified index. Will have to match the data type that the array is set at (e.g, int32, float).
    #
    # Example:
    #
    # @code
    # myarray.set(3) = 10       # 1D array
    # myarray.set([1,3]) = 10   # 2D array
    # myarray.set([2,1,1]) = 10 # 3D array
    # @endcode
    def set(self, idx, value):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx

        idx_arr = (c_size_t * len(idx))(*idx)

        if self.dtype == 'int32':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_int32(value)))
        elif self.dtype == 'float':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_float(value)))

    ## Set an element by index.
    # This is a wrapper around the zumpy.array.set(self, idx, value) method to use convenient square bracket syntax.
    # @param idx A list specifying the index. E.g [1, 2] will access the element at the second row and third column (zero-indexed).
    #
    # Example:
    #
    # @code
    # myarray[3] = 10     # 1D array
    # myarray[1,2] = 10   # 2D array
    # myarray[2,1,1] = 10 # 3D array
    # @endcode
    def __setitem__(self, idx, value):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx
        self.set(temp_idx, value)

    ## Fill all cells with a specified value
    # This will set every index of the array to the same value.
    # @param value Value to set all indices to
    #
    # Example:
    #
    # @code
    # from zumpy import array
    #
    # # this will fill a 3x2 array with 10s
    # myarray = array([3,2], 'int32')
    # myarray.fill(10)
    # @endcode
    def fill(self, value):
        val_ptr = None
        if self.dtype == 'int32':
            val_ptr = cast(byref(c_int32(value)), c_void_p)
        elif self.dtype == 'float':
            val_ptr = cast(byref(c_float(value)), c_void_p)
        _libZumpy.arr_fill(byref(self.arr), val_ptr)

    ## Slice an array to extract subsets
    # @param slice_indices A list of lists containing the indices to slice. First dimension corresponds to the array dimension and second dimension corresponds to the indices to pull from that dimension. See example below.
    #
    # Example:
    #
    # @code
    # from zumpy import array
    #
    # arr = array()
    # arr.create([3,2], 'int32')
    # arr.fill(10)
    #
    # print("Full Array:")
    # print(arr)
    #
    # # reading the parameter explicitly, this is saying take index 0-2 from array dimension 0
    # # and take index 0 from array dimension 1.
    # # In other words, take all rows from column 0.
    # sub = arr.slice([range(3), [0]])
    # print("Sliced Array:")
    # print(sub)
    # @endcode
    #
    # Output:
    #
    # @code
    # Full Array:
    # 10 10
    # 10 10
    # 10 10
    #
    # Sliced Array:
    # 10
    # 10
    # 10
    # @endcode
    #
    # Example 2:
    #
    # @code
    # from zumpy import array
    #
    # arr = array()
    # arr.create([3,2,3], 'int32')
    # arr.fill(10)
    # # set the right-most column to 20
    # for i in range(arr.shape[0]):
    #     for j in range(arr.shape[1]):
    #         arr[i,j,2] = 20
    #
    # print("Full Array:")
    # print(arr)
    #
    # # take all indices from dimension 0,
    # # all indices from dimension 1,
    # # and index 2 from dimension 2.
    # # In other words, this will extract the right-most column in a 3D array.
    # sub = arr.slice([range(3), range(2), [2]])
    # print("Sliced Array:")
    # print(sub)
    # @endcode
    #
    # Output:
    #
    # @code
    # Full Array:
    # 10 10 20
    # 10 10 20
    #
    # 10 10 20
    # 10 10 20
    #
    # 10 10 20
    # 10 10 20
    #
    #
    # Sliced Array:
    # 20
    # 20
    #
    # 20
    # 20
    #
    # 20
    # 20
    # @endcode
    def slice(self, slice_indices):
        # slice indices should be a list of lists
        # convert slice_indices to size_t** (pointer to pointer of size_t)
        arr_inner = []
        for i in range(len(slice_indices)):
            arr_inner.append((c_size_t * len(slice_indices[i]))(*slice_indices[i]))

        arr_outer = []
        for i in range(len(arr_inner)):
            arr_outer.append(arr_inner[i])

        pp_slice_indices = (POINTER(c_size_t) * len(arr_outer))(*arr_outer)

        slice_idx_len = len(slice_indices)
        slice_dims = []
        for idx in slice_indices:
            slice_dims.append(len(idx))

        ref_arr = array_wrapper()
        p_slice_dims = (c_size_t * len(slice_dims))(*slice_dims)
        _libZumpy.arr_slice(byref(self.arr), pp_slice_indices, p_slice_dims, c_size_t(slice_idx_len), byref(ref_arr))

        ret_arr = array(slice_dims, self.dtype)
        ret_arr.arr = ref_arr

        return ret_arr

    ## Filter an array based on user-defined condition.
    # @note You will need to use ctypes in the filter function to convert values so the underlying C code knows what to do.
    # @note Currently this filter doesn't support different filters on different columns simultaneously, but that's planned soon.
    # @param filter_func A user-defined python function that takes one parameter and returns a boolean. You will need to use ctypes to convert this parameter into your array type. See example below.
    # @param secondary_indices These are the indices to restrict the filter to and are analogous to columns. E.g if you pass [1] it will only check the filter against column 1. If you pass an empty list [], it will check all columns.
    # @param filter_type A string specifying 'ANY' or 'ALL'. This only applies to arrays 2D or above and if you are applying the filter to multiple columns. If 'ANY' is used, then the filter must pass (be true) for AT LEAST one of the columns; then that row will be returned. If 'ALL' is used, then ALL columns must satisfy the filter in order for that row to be returned.
    #
    # Example:
    #
    # @code
    # from zumpy import array
    # from ctypes import *
    # from random import randint, seed
    #
    # # currently don't know a better way to make this more user-friendly
    # # so for now, you will have to use a bit of ctypes magic
    # def myfilter(x):
    #     x = cast(x, POINTER(c_int32)) # cast parameter to a pointer of our array type (int32)
    #     return x.contents.value > 20  # dereference and access the pointer value and check the condition
    #
    # arr = array()
    # arr.create([5,2], 'int32')
    #
    # # set seed for reproducibility
    # seed(5021)
    #
    # # fill array with random values
    # for i in range(arr.shape[0]):
    #     for j in range(arr.shape[1]):
    #         arr[i,j] = randint(0,50)
    #
    # print("Full Array:")
    # print(arr)
    #
    # # this will check if EITHER column 0 or 1 match the condition
    # # we are passing an empty list in second parameter to check all columns.
    # print("Filtered ANY:")
    # filtered_any = arr.filter(myfilter, [], 'ANY')
    # print(filtered_any)
    #
    # # this will check if BOTH column 0 and 1 match the condition
    # print("Filtered ALL:")
    # filtered_all = arr.filter(myfilter, [], 'ALL')
    # print(filtered_all)
    # @endcode
    #
    # Output:
    #
    # @code
    # Full Array:
    # 37 39
    # 32 21
    # 49 44
    # 0 35
    # 12 18
    #
    # Filtered ANY:
    # 37 39
    # 32 21
    # 49 44
    # 0 35
    #
    # Filtered ALL:
    # 37 39
    # 32 21
    # 49 44
    #
    # @endcode
    def filter(self, filter_func, secondary_indices, filter_type):
        proto_filter_func = CFUNCTYPE(c_bool, c_void_p)
        p_filter_func = proto_filter_func(filter_func)

        p_secondary_indices = None
        if len(secondary_indices) != 0:
            p_secondary_indices = (c_size_t * len(secondary_indices))(*secondary_indices)

        ftype = None
        if (filter_type == 'ANY'):
            ftype = 0
        elif (filter_type == 'ALL'):
            ftype = 1

        dest_arr = array_wrapper()

        _libZumpy.arr_filter(byref(self.arr), p_filter_func, p_secondary_indices, c_size_t(len(secondary_indices)), c_uint(ftype), byref(dest_arr))

        # grab contents from struct pointer and convert array
        # shape into Python list
        _shape_size = pointer(dest_arr).contents.shape_size
        _arr_shape = pointer(dest_arr).contents.arr_shape
        _total_size = pointer(dest_arr).contents.total_size
        _arr_shape_list = []
        for i in range(_shape_size):
            _arr_shape_list.append(_arr_shape[i])
        ret_arr = array(_arr_shape_list, self.dtype)
        ret_arr.arr = dest_arr

        # return NULL if filter returned no results
        if _total_size == 0:
            return None
        return ret_arr

    ## Sum all indices of an array
    # @return A float value representing the sum of all the elements
    #
    # Example:
    #
    # @code
    # from zumpy import array
    # from random import randint, seed
    #
    # arr = array()
    # arr.create([5,2], 'int32')
    #
    # # set seed for reproducibility
    # seed(5021)
    #
    # # fill array with random values
    # for i in range(arr.shape[0]):
    #     for j in range(arr.shape[1]):
    #         arr[i,j] = randint(0,50)
    #
    # print("Full Array:")
    # print(arr)
    #
    # print("Sum: ", arr.sum())
    # @endcode
    #
    # Output:
    #
    # @code
    # Full Array:
    # 37 39
    # 32 21
    # 49 44
    # 0 35
    # 12 18
    #
    # Sum:  287.0
    # @endcode
    def sum(self):
        return _libZumpy.arr_sum(byref(self.arr))