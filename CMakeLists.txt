cmake_minimum_required(VERSION 2.8.8)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -std=gnu++11")
endif()

add_subdirectory(src)
