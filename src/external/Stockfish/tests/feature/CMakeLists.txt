cmake_minimum_required(VERSION 2.8.8)

include_directories(../../src)
include_directories(../../src/syzygy)

add_executable(testfeat main.cpp)

target_link_libraries(testfeat stockfish-static syzygy-static)
