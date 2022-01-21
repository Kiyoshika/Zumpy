cmake_minimum_required(VERSION 3.21)
project(Zumpy C)

set(CMAKE_C_STANDARD 99)

add_library(Zumpy SHARED src/c/zumpy.c)
add_executable(testing src/c/main.c)
target_link_libraries(testing Zumpy)
