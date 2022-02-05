cmake_minimum_required(VERSION 3.21)
project(Zumpy C)

set(CMAKE_C_STANDARD 99)

add_library(Zumpy SHARED src/c/zumpy.c src/c/access.c src/c/maths.c src/c/slice.c src/c/print.c src/c/filter.c src/c/zumpy_internal.c)
add_executable(testing src/c/main.c src/c/access.c src/c/maths.c src/c/slice.c src/c/print.c src/c/filter.c src/c/zumpy_internal.c)
target_link_libraries(testing Zumpy)
