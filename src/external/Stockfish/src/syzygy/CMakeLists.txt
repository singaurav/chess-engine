cmake_minimum_required(VERSION 2.8.8)

set(SYZYGY_SRCS tbprobe.cpp)

add_library(syzygy-static STATIC ${SYZYGY_SRCS})
