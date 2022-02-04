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

class array():
    def _get_type_enum(self, dtype):
        if dtype == 'int32':
            return 0
        elif dtype == 'float':
            return 1

    arr = None
    dtype = None
    shape = None

    def create(self, shape, dtype = 'int32'):

        self.arr = array_wrapper()
        self.dtype = dtype
        self.shape = shape

        arr_ptr = pointer(self.arr)

        shape_size = len(self.shape)
        shape_arr = (c_size_t * len(self.shape))(*self.shape)

        type_enum = self._get_type_enum(self.dtype)

        _libZumpy.arr_init(arr_ptr, shape_arr, shape_size, type_enum)

    def __init__(self, shape = None, dtype = 'int32'):
        if shape != None:
            self.create(shape, dtype)

    def __del__(self):
        arr_ptr = pointer(self.arr)
        _libZumpy.arr_free(arr_ptr)

    # override the print() call
    def __str__(self):
        arr_ptr = pointer(self.arr)
        _libZumpy.arr_print(arr_ptr)
        return ""

    def __repr__(self):
        self.__str__()

    def at(self, idx):
        idx_arr = (c_size_t * len(idx))(*idx)
        # dereference different types
        if self.dtype == 'int32':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_int32)).contents.value
        elif self.dtype == 'float':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_float)).contents.value

        return None

    def __getitem__(self, idx):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx
        return self.at(temp_idx)

    def set(self, idx, value):
        idx_arr = (c_size_t * len(idx))(*idx)

        if self.dtype == 'int32':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_int32(value)))
        elif self.dtype == 'float':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_float(value)))

    def __setitem__(self, idx, value):
        temp_idx = []
        if isinstance(idx, int):
            temp_idx.append(idx)
        else:
            temp_idx = idx
        self.set(temp_idx, value)

    def fill(self, value):
        val_ptr = None
        if self.dtype == 'int32':
            val_ptr = cast(byref(c_int32(value)), c_void_p)
        elif self.dtype == 'float':
            val_ptr = cast(byref(c_float(value)), c_void_p)
        _libZumpy.arr_fill(byref(self.arr), val_ptr)

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


    def sum(self):
        return _libZumpy.arr_sum(byref(self.arr))