# python binding for libZumpy.so
from ctypes import *

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

    def at(self, idx):
        idx_arr = (c_size_t * len(idx))(*idx)
        # dereference different types
        if self.dtype == 'int32':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_int32)).contents.value
        elif self.dtype == 'float':
            return cast(cast(_libZumpy.arr_at(byref(self.arr), idx_arr), c_void_p), POINTER(c_float)).contents.value

        return None

    def __getitem__(self, idx):
        return self.at(idx)

    def set(self, idx, value):
        idx_arr = (c_size_t * len(idx))(*idx)

        if self.dtype == 'int32':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_int32(value)))
        elif self.dtype == 'float':
            _libZumpy.arr_set(byref(self.arr), idx_arr, byref(c_float(value)))

    def __setitem__(self, idx, value):
        self.set(idx, value)

    def fill(self, value):
        val_ptr = None
        if self.dtype == 'int32':
            val_ptr = cast(byref(c_int32(value)), c_void_p)
        elif self.dtype == 'float':
            val_ptr = cast(byref(c_float(value)), c_void_p)
        _libZumpy.arr_fill(byref(self.arr), val_ptr)

    def sum(self):
        return _libZumpy.arr_sum(byref(self.arr))