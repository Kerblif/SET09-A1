cmake_minimum_required(VERSION 3.28)
project(SET09-A1 CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

add_executable(A1
        merge.cpp
        quick.cpp
        radix.cpp
        radix+quick.cpp
        sort.h
        string_generator.cpp
        string_generator.h
        string_sort_tester.h
        string_sort_tester.cpp
        main.cpp
)