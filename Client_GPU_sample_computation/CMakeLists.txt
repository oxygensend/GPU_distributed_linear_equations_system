cmake_minimum_required(VERSION 3.24)
project(project CUDA)

set(CMAKE_CUDA_STANDARD 17)

add_executable(project main.cu main2.cu)

set_target_properties(project PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcuda")